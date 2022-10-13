// bslstl_list.2.t.cpp                                                -*-C++-*-
#define BSLSTL_LIST_0T_AS_INCLUDE
#include <bslstl_list.0.t.cpp>

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// See the test plan in 'bslstl_list.0.t.cpp'.

// ============================================================================
//              ADDITIONAL TEST MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION >= 0x5130
// Some compilers struggle with the number of template instantiations in this
// test driver.  We define this macro to simplify the test driver for them,
// until such time as we can provide a more specific review of the type based
// concerns, and narrow the range of tests needed for confirmed coverage.
//
// Currently we are enabling the minimal set of test types on:
// Sun Studio 12.4            (CMP_SUN)
// (note: despite over-eager version check, we have not tested later compilers)
# define BSLSTL_LIST_TEST_LOW_MEMORY     1
#endif

#if defined(BSLSTL_LIST_TEST_LOW_MEMORY)
// For platforms that cannot sustain the full set of test concerns, reduce the
// number of elements in the most commonly use macro defining sets of test
// tyoes.
# undef  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR
# define BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR  \
        signed char,                                    \
        bsltf::TemplateTestFacility::MethodPtr,         \
        bsltf::AllocBitwiseMoveableTestType,            \
        bsltf::MovableAllocTestType
#endif

//=============================================================================
//                          TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestDriver2 : TestSupport<TYPE, ALLOC> {

                     // NAMES FROM DEPENDENT BASE

    BSLSTL_LIST_0T_PULL_TESTSUPPORT_NAMES;
    // Unfortunately the names have to be made available "by hand" due to two
    // phase name lookup not reaching into dependent bases.

                            // TEST CASES
    // CLASS METHODS
    static void test12_constructorRange();
    template <class CONTAINER>
    static void test12_constructorRange(const CONTAINER&);
        // Test user-supplied constructor templates.  The second method calls
        // the first with a container capable of storing a range of objects of
        // type 'TYPE'.  Not that the container passed is never used (hence
        // unnamed), it just determines the type of container to be built by
        // the test case, and hence what type of iterators can be used to
        // traverse it.

    static void test12_initialLengthConstructor();
    static void test12_initialLengthConstructor(bsl::true_type);
    static void test12_initialLengthConstructor(bsl::false_type);
    static void test12_initialLengthConstructorDefault();
    static void test12_initialLengthConstructorNonDefault();
        // Test user-supplied initial-length constructors, where
        // 'test12_initialLengthConstructorDefault' tests the c'tor where no
        // initial value is supplied, where
        // 'test12_initialLengthConstructorNonDefault' tests the c'tor where a
        // default value is supplied (and hence a copy c'tor of 'TYPE' must be
        // available, and the last 3 are just dispatch functions.

    static void test13_initialLengthAssign();
        // Test 'assign' members.

    static void test13_assignRange();
    template <class CONTAINER>
    static void test13_assignRange(const CONTAINER&);
        // Test 'assign' member template.  The second method calls the first
        // with a container capable of storing a range of objects of type
        // 'TYPE'.  Not that the container passed is never used (hence
        // unnamed), it just determines the type of container to be built by
        // the test case, and hence what type of iterators can be used to
        // traverse it.

    static void test14_resize();
    static void test14_resize(bsl::false_type hasNoCopyCtor);
    static void test14_resize(bsl::true_type hasNoCopyCtor);
    static void test14_resizeDefault();
    static void test14_resizeNonDefault();
        // Test 'resize' and 'max_size'

    static void test15_elementAccess();
        // Test element access.

    static void test16_iterators();
        // Test iterators.

    static void test17_emplace();
        // Test 'emplace', 'emplace_front', and 'emplace_back' members.  The
        // first function will only work if 'TYPE' has a constructor with 4
        // extra integer arguments.  The second function is a no-op.  The third
        // function is a dispatcher.  Note that this method can only be called
        // if 'TYPE' has a c'tor that will take 0-4 integer arguments.

    static void test17_insert();
        // Test 'insert', 'push_back', and 'push_front' members.

    static void test17_insertRange();
    template <class CONTAINER>
    static void test17_insertRange(const CONTAINER&);
        // Test 'insert' member template.  The second method calls the first
        // with a container capable of storing a range of objects of type
        // 'TYPE'.  Not that the container passed is never used (hence
        // unnamed), it just determines the type of container to be built by
        // the test case, and hence what type of iterators can be used to
        // traverse it.

    static void test18_erase();
        // Test 'erase' and 'pop_back'.

    template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG, bool OTHER_FLAGS>
    static void test19_propagate_on_container_swap_dispatch();
    static void test19_propagate_on_container_swap();
        // Test 'propagate_on_container_swap'.

    static void test19_swap_noexcept();
        // Test noexcept specification of 'swap'.

    static void test19_swap();
        // Test 'swap' member and global swap.

    static void test20_comparisonOps();
    static void test20_comparisonOps(bsl::false_type);
    static void test20_comparisonOps(bsl::true_type);
        // Test comparison free operators.  The method that takes a 'true_type'
        // argument is called when 'TYPE' supports 'operator<' and it does the
        // testing.  The method that takes a 'false_type' is called when 'TYPE'
        // does not, and it is a no-op.  The method with no arguments is a
        // dispatcher between the two.

    static void test21_typedefs();
        // Verify (at compile time) that the types of all the typedefs are
        // correct.

    static void test22_typeTraits();
        // Test type traits

    static void test23_reverse();
        // Test 'reverse'

    static void test24_splice();
        // Test 'splice'

    static void test25_remove();
        // Test 'remove' and 'remove_if'

    static void test26_unique();
        // Test 'unique'

    static void test27_merge();
    static void test27_merge(bsl::false_type);
    static void test27_merge(bsl::true_type);
    static void test27_mergeComp();
    static void test27_mergeNoComp();
        // Test 'merge'.  'test27_mergeNoComp' tests a merge where no
        // comparator is passed.  It must be called only if 'operator<' is
        // defined on 'TYPE'.  'test27_mergeComp' will pass a comparator
        // object, and hence will work for the full suite of test types.
        // 'test27_merge(false_type)' is to be called for types with no
        // 'operator<', it will call 'test27_mergeNoComp' only.
        // 'test27_merge(true_type)' calls both 'test27_mergeNoComp' and
        // 'test27_mergeComp'.  'test27_merge' is the dispatcher function.

    static void test28_sort();
    static void test28_sort(bsl::true_type hasOperatorLessThan);
    static void test28_sort(bsl::false_type hasOperatorLessThan);
    static void test28_sortComp();
    static void test28_sortNoComp();
        // Test 'sort'.  'test28_sortNoComp' calls 'sort' without passing it a
        // comparator object, and will only compile if 'TYPE' has 'operator<'
        // defined.  'test28_sortComp' calls 'sort' with a comparator and is
        // called for all types.  The 3 'test28_sort' functions are just
        // dispatcher functions.

    static void test29_moveInsert();
        // Test move-based inserters, 'insert', 'push_back', 'push_front',
        // 'emplace', 'emplace_front', 'emplace_back', with movable arguments.

    static void test30_moveCtor();
        // Test move c'tors, both with and without an allocator passed.

    template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void test31_propagate_on_container_move_assignment_dispatch();
    static void test31_propagate_on_container_move_assignment();
        // Test 'propagate_on_container_move_assignment'.

    static void test31_moveAssign_noexcept();
        // Test noexcept specification of move assign.

    static void test31_moveAssign();
        // Test move assign.

    static void test33_initializerList();
        // Test initializer lists.

    static void testCase34_noexcept();
        // Test 'noexcept' specifications

    static void testCase36_erase();
        // Test free functions 'erase' and 'erase_if'
};

                  // ==================================
                  // template class StdBslmaTestDriver2
                  // ==================================

template <class TYPE>
class StdBslmaTestDriver2 : public StdBslmaTestDriverHelper<TestDriver2, TYPE>
{
};

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

                                 // ----------
                                 // TEST CASES
                                 // ----------

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase36_erase()
    // test 'bsl::erase' and 'bsl::erase_if' with 'bsl::list'.
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
void TestDriver2<TYPE, ALLOC>::testCase34_noexcept()
{
    // ------------------------------------------------------------------------
    // 'noexcept' SPECIFICATION
    //
    // Concerns:
    //: 1 The 'noexcept' specification has been applied to all class interfaces
    //:   required by the standard.
    //
    // Plan:
    //: 1 Apply the unary 'noexcept' operator to expressions that mimic those
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

    // N4594: page 846: 23.3.10 Class template 'list' [list]

    // page 847
    //..
    //  // 23.3.10.2, construct/copy/destroy:
    //  list& operator=(list&& x)
    //      noexcept(allocator_traits<Allocator>::is_always_equal::value);
    //  allocator_type get_allocator() const noexcept;
    //..

    {
        Obj mX;  const Obj& X = mX;
        Obj mY;

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX =
                                             bslmf::MovableRefUtil::move(mY)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.get_allocator()));
    }

    // page 847
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
        Obj mX; const Obj& X = mX;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.begin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.begin()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.end()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.end()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.rbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.rbegin()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.rend()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.rend()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.cbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.cend()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.crbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.crend()));
    }

    // page 848
    //..
    //  // 23.3.10.3, capacity:
    //  bool empty() const noexcept;
    //  size_type size() const noexcept;
    //  size_type max_size() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.empty()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.size()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.max_size()));
    }

    // page 848
    //..
    //  // 23.3.10.4, modifiers:
    //  void swap(list&)
    //     noexcept(allocator_traits<Allocator>::is_always_equal::value);
    //  void clear() noexcept;
    //..

    {
        Obj x;
        Obj y;

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.swap(y)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.clear()));
    }

    // page 848-849
    //..
    //  // 23.3.10.5, list operations:
    //  void reverse() noexcept;
    //..

    {
        Obj x;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.reverse()));
    }

    // page 849
    //..
    //  // 23.3.10.6, specialized algorithms:
    //  template <class T, class Allocator>
    //  void swap(list<T, Allocator>& x, list<T, Allocator>& y)
    //      noexcept(noexcept(x.swap(y)));
    //..

    {
        Obj x;
        Obj y;

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.swap(y)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.clear()));
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test33_initializerList()
{
    // ------------------------------------------------------------------------
    // TESTING INITIALIZER LIST
    //
    // Concerns:
    //: 1 That lists can be constructed from an initializer list.
    //:   o Without an allocator passed.
    //:   o With an allocator passed.
    //:
    //: 2 That lists can be assigned from an initializer list, either through
    //:   o 'operator='
    //:   o 'assign'
    //:
    //: 3 That after the operation, the values are correct.
    //:
    //: 4 That if a c'tor throws, no memory is leaked.
    //:
    //: 5 That if an 'operator=' or 'assign' throws, the destination is left in
    //:   a valid state (though it may be changed).
    //:
    //: 6 That the expected amount of memory is allocated.
    //:
    //: 7 If the list is passed a non-default allocator at construction, the
    //:   default allocator is not used.
    //:
    //: 8 'operator=' returns a reference to the modifiable object assigned to.
    //
    // Plan:
    //: 1 Whenever possible, do the tests in exception blocks.  (C-4)
    //:   o In the case of assigns (either type), check the integrity of the
    //:     destination early in the block to verify its integrity immediately
    //:     after a throw.  (C-5)
    //:   o in the case of c'tors, make sure any allocator passed, and the
    //:     default allocator, are bslma::TestAllocators, which will detect any
    //:     leaks.  (C-4)
    //:
    //: 2 Create an 'expected' value, do the operation, and compare the result
    //:   to the expected value.  In cases of assignment, make sure the initial
    //:   state of the object is either always (in the case of not being done
    //:   in loops) or usually (in the case of done in loops) different from
    //:   the expected value.  (C-1) (C-2) (C-3)
    //:
    //: 3 Monitor the number of allocations and the blocks in use to verify the
    //:   amount of allocation.  (C-6)
    //:
    //: 4 When the test object is created with a non-default allocator, observe
    //:   that the default allocator is never used.  (C-7)
    //
    // Testing
    //   list(std::initializer_list, const A& = ALLOCATOR());
    //   list& operator=(std::initializer_list);
    //   void assign(std::initializer_list);
    // ------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    if (verbose) {
        printf("TESTING INITIALIZER LIST: TYPE: %s\n"
               "==================================\n",
               bsls::NameOf<TYPE>().name());
    }

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    const ALLOC          xoa(&oa);

    TestValues V("ABCDEFGH");

    // Some (or all) of the types that allocate memory allocate on assign.
    // Measure how much they do and store it in 'allocOnAssign'.

    Int64 allocOnAssign = 0;
    if (k_SCOPED_ALLOC) {
        // 'checkIntegrity' default-constructs an object of type 'TYPE' to get
        // it's value, but only the first time it's called, which can be
        // problematic if 'TYPE' allocates, so call it once here to get that
        // default allocation out of the way prior to our other tests.

        Obj mX;  const Obj& X = mX;
        (void) checkIntegrity(X, 0);

        mX.resize(2);

        Int64 BB = defaultAllocator_p->numBlocksTotal();

        mX.front() = mX.back();

        allocOnAssign = defaultAllocator_p->numBlocksTotal() - BB;
    }

    // First check without loop, instead of passing ref.

    {
        ALLOC DZ(defaultAllocator_p);

        Obj expected;  const Obj& EXPECTED = gg(&expected, "ECA");

        Int64 B = defaultAllocator_p->numBlocksInUse(), A, BB, AA;

        int numThrows;
#if defined(BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS)
        // To avoid reporting spurious memory leaks, perform this test without
        // exceptions.

        if (veryVerbose) {
            printf("C'tor, naked list, %s, no exceptions\n",
                   bsls::NameOf<TYPE>().name());
        }
        {
            BB = defaultAllocator_p->numBlocksTotal();

            Obj mX = { V[4], V[2], V[0] };  const Obj& X = mX;

            AA = defaultAllocator_p->numBlocksTotal();
            A  = defaultAllocator_p->numBlocksInUse();

            if (!k_SCOPED_ALLOC) {
                // If the initializer list allocates memory, then it's too
                // hard to predict, since, depending on the compiler, it might
                // or might not have been freed by now.

                ASSERTV(A - B, bsls::NameOf<TYPE>(),
                        A - B  == deltaBlocks(3) + 1);
                ASSERTV(A - B, AA - BB, A - B == AA - BB);
            }

            ASSERTV(EXPECTED, X, EXPECTED == X);

            ASSERT(checkIntegrity(X, 3));

            ASSERT(DZ == X.get_allocator());
        }
#else
        // I have tried to make this code correct; however, it has never been
        // compiled and run as we don't currently have a correct C++11 compiler
        // with which to do so.

        if (veryVerbose) {
            printf("C'tor, naked list, %s + exceptions\n",
                   bsls::NameOf<TYPE>().name());
        }

        numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*defaultAllocator_p) {
            ++numThrows;

            BB = defaultAllocator_p->numBlocksTotal();

            Obj mX = { V[4], V[2], V[0] };  const Obj& X = mX;

            AA = defaultAllocator_p->numBlocksTotal();
            A  = defaultAllocator_p->numBlocksInUse();

            ASSERTV(AA - BB, numThrows, k_NO_EXCEPT || AA - BB == numThrows);
            if (!k_SCOPED_ALLOC) {
                // If the initializer list allocates memory, then it's too hard
                // to predict, since, depending on the compiler, it might or
                // might not have been freed by now.

                ASSERTV(A - B, bsls::NameOf<TYPE>(),
                        A - B  == deltaBlocks(3) + 1);
                ASSERTV(A - B, AA - BB, A - B == AA - BB);
            }

            ASSERTV(EXPECTED, X, EXPECTED == X);

            ASSERT(checkIntegrity(X, 3));

            ASSERT(DZ == X.get_allocator());
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#endif

        A = defaultAllocator_p->numBlocksInUse();
        ASSERTV(bsls::NameOf<TYPE>(), A - B, A == B);

        Int64 DB = defaultAllocator_p->numBlocksTotal();

        if (veryVerbose) {
            printf("Ctor, list w/ alloc + excep, %s\n",
                   bsls::NameOf<TYPE>().name());
        }

        B = oa.numBlocksInUse();

        numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            ++numThrows;

            BB = oa.numBlocksTotal();

            Obj mX({ V[4], V[2], V[0] }, xoa);  const Obj& X = mX;

            AA = oa.numBlocksTotal();
            A  = oa.numBlocksInUse();

            ASSERTV(k_NO_EXCEPT || AA - BB == numThrows);
            ASSERT(A  - B  == deltaBlocks(3) + 1);
            ASSERTV(A - B, AA - BB, A - B == AA - BB);

            ASSERTV(EXPECTED, X, EXPECTED == X);
            ASSERT(checkIntegrity(X, 3));

            ASSERT(xoa == X.get_allocator());
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        A  = oa.numBlocksInUse();
        ASSERTV(bsls::NameOf<TYPE>(), A - B, A == B);

        // The code made 'numThrows + 1' passes through the exception-block.
        // It built 3 'TYPE' objects for the initializer list each time.

        Int64 DA = defaultAllocator_p->numBlocksTotal();
        ASSERTV(bsls::NameOf<TYPE>(), DB, DA, numThrows,
               k_NO_EXCEPT || DA - DB == (numThrows + 1) * k_SCOPED_ALLOC * 3);

        if (veryVerbose) {
            printf("Op=, naked list, %s + exceptions\n",
                   bsls::NameOf<TYPE>().name());
        }

        Obj mX(xoa);  const Obj& X = mX;
        Obj mY(xoa);  const Obj& Y = gg(&mY, "GA");

        DB = defaultAllocator_p->numBlocksTotal();
        numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            ++numThrows;

            const Int64 AL = oa.allocationLimit();
            oa.setAllocationLimit(-1);

            ASSERT(checkIntegrity(X, X.size()));
            mX = Y;

            oa.setAllocationLimit(AL);

            BB = oa.numBlocksTotal();
            B  = oa.numBlocksInUse();

            Obj& mRef = (mX = { V[4], V[2], V[0] });

            AA = oa.numBlocksTotal();
            A  = oa.numBlocksInUse();

            ASSERT(&mRef == &mX);
            ASSERTV(AA - BB, numThrows, k_NO_EXCEPT || AA - BB == numThrows);
            ASSERTV(A - B, bsls::NameOf<TYPE>(), A - B == deltaBlocks(3-2));
            ASSERTV(A - B, AA - BB, bsls::NameOf<TYPE>(), allocOnAssign,
                    A - B + 2 * allocOnAssign == AA - BB);

            ASSERTV(EXPECTED, X, EXPECTED == X);

            ASSERT(checkIntegrity(X, 3));

            ASSERT(xoa == X.get_allocator());
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        // The code made 'numThrows + 1' passes through the exception-block.
        // It built 3 'TYPE' objects for the initializer list each time.

        DA = defaultAllocator_p->numBlocksTotal();
        ASSERTV(bsls::NameOf<TYPE>(), DB, DA, numThrows,
               k_NO_EXCEPT || DA - DB == (numThrows + 1) * k_SCOPED_ALLOC * 3);

        if (veryVerbose) {
            printf("'assign', naked list, %s + exceptions\n",
                   bsls::NameOf<TYPE>().name());
        }
        DB = defaultAllocator_p->numBlocksTotal();
        numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            ++numThrows;

            const Int64 AL = oa.allocationLimit();
            oa.setAllocationLimit(-1);

            ASSERT(checkIntegrity(X, X.size()));
            mX = Y;

            oa.setAllocationLimit(AL);

            BB = oa.numBlocksTotal();
            B  = oa.numBlocksInUse();

            mX.assign({ V[4], V[2], V[0] });

            AA = oa.numBlocksTotal();
            A  = oa.numBlocksInUse();

            ASSERTV(AA - BB, numThrows, k_NO_EXCEPT || AA - BB == numThrows);
            ASSERTV(A - B, bsls::NameOf<TYPE>(), A - B == deltaBlocks(3-2));
            ASSERTV(A - B, AA - BB, bsls::NameOf<TYPE>(), allocOnAssign,
                    A - B + 2 * allocOnAssign == AA - BB);

            ASSERTV(EXPECTED, X, EXPECTED == X);

            ASSERT(checkIntegrity(X, 3));

            ASSERT(xoa == X.get_allocator());
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        // The code made 'numThrows + 1' passes through the exception-block.
        // It built 3 'TYPE' objects for the initializer list each time.

        DA = defaultAllocator_p->numBlocksTotal();
        ASSERTV(bsls::NameOf<TYPE>(), DB, DA, numThrows,
               k_NO_EXCEPT || DA - DB == (numThrows + 1) * k_SCOPED_ALLOC * 3);
    }

    const struct {
        int                          d_line;   // source line number
        std::initializer_list<TYPE>  d_list;   // source list
        const char                  *d_spec_p; // expected result
    } DATA[] = {
        //line  list                       result
        //----  -----------------------    ------

        { L_,  {                        }, ""     },
        { L_,  { V[0]                   }, "A"    },
        { L_,  { V[6], V[6]             }, "GG"   },
        { L_,  { V[1], V[0]             }, "BA"   },
        { L_,  { V[5], V[3], V[2]       }, "FDC"  },
        { L_,  { V[2], V[1], V[0]       }, "CBA"  },
        { L_,  { V[3], V[1], V[0], V[2] }, "DBAC" },
        { L_,  { V[1], V[5], V[4], V[4] }, "BFEE" },
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const std::initializer_list<TYPE>&  LIST   = DATA[ti].d_list;
        const char                         *SPEC   = DATA[ti].d_spec_p;
        const IntPtr                        LENGTH = std::strlen(SPEC);

        Obj expected;  const Obj& EXPECTED = gg(&expected, SPEC);

        if (veryVerbose)
            printf("C'tor, init list, %s\n", bsls::NameOf<TYPE>().name());
        {
            ALLOC       DZ(defaultAllocator_p);
            const Int64 B = defaultAllocator_p->numBlocksInUse();

            int numThrows = -1;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*defaultAllocator_p) {
                ++numThrows;

                const Int64 BB = defaultAllocator_p->numBlocksTotal();

                Obj mX = LIST;    const Obj& X = mX;

                const Int64 AA = defaultAllocator_p->numBlocksTotal();
                const Int64 A  = defaultAllocator_p->numBlocksInUse();

                ASSERTV(SPEC, AA - BB, numThrows,
                        k_NO_EXCEPT || AA - BB == numThrows);
                ASSERTV(A  - B  == deltaBlocks(LENGTH) + 1);
                ASSERTV(A - B, AA - BB, A - B == AA - BB);
                ASSERTV(EXPECTED, X, EXPECTED == X);
                ASSERTV(checkIntegrity(X, LENGTH));
                ASSERTV(DZ == X.get_allocator());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        const Int64 DD = defaultAllocator_p->numBlocksTotal();

        if (veryVerbose)
            printf("C'tor, init list w/ alloc, %s\n",
                   bsls::NameOf<TYPE>().name());
        {
            const Int64 B = oa.numBlocksInUse();

            int numThrows = -1;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numThrows;

                const Int64 BB = oa.numBlocksTotal();

                Obj mX(LIST, xoa); const Obj& X = mX;

                const Int64 AA = oa.numBlocksTotal();
                const Int64 A  = oa.numBlocksInUse();

                ASSERTV(k_NO_EXCEPT || AA - BB == numThrows);
                ASSERTV(A  - B  == deltaBlocks(LENGTH) + 1);
                ASSERTV(A - B, AA - BB, A - B == AA - BB);
                ASSERTV(EXPECTED, X, EXPECTED == X);
                ASSERT(checkIntegrity(X, LENGTH));
                ASSERT(xoa == X.get_allocator());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        ASSERTV(SPEC,
                bsls::NameOf<TYPE>(),
                DD,
                defaultAllocator_p->numBlocksTotal(),
                DD == defaultAllocator_p->numBlocksTotal());

        if (veryVerbose)
            printf("op=, init list w/ alloc, %s\n",
                   bsls::NameOf<TYPE>().name());

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const char   *DST_SPEC   = DATA[tj].d_spec_p;
            const IntPtr  DST_LENGTH = std::strlen(DST_SPEC);

            Obj mX(xoa); const Obj& X = mX;
            Obj mY(xoa); const Obj& Y = gg(&mY, DST_SPEC);

            int numThrows = -1;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numThrows;

                const Int64 AL = oa.allocationLimit();
                oa.setAllocationLimit(-1);

                ASSERT(checkIntegrity(X, X.size()));
                mX = Y;

                oa.setAllocationLimit(AL);

                const Int64 BB = oa.numBlocksTotal();
                const Int64 B  = oa.numBlocksInUse();

                mX = LIST;

                const Int64 AA = oa.numBlocksTotal();
                const Int64 A  = oa.numBlocksInUse();

                ASSERTV(k_NO_EXCEPT || AA - BB == numThrows);
                ASSERTV(A - B == deltaBlocks(LENGTH - DST_LENGTH));

                ASSERTV(EXPECTED, X, EXPECTED == X);
                ASSERTV(checkIntegrity(X, LENGTH));

                ASSERTV(xoa == X.get_allocator());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        if (veryVerbose)
            printf("assign, init list w/ alloc, %s\n",
                   bsls::NameOf<TYPE>().name());

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const char   *DST_SPEC   = DATA[tj].d_spec_p;
            const IntPtr  DST_LENGTH = std::strlen(DST_SPEC);

            Obj mX(xoa);  const Obj& X = mX;
            Obj mY(xoa);  const Obj& Y = gg(&mY, DST_SPEC);

            int numThrows = -1;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numThrows;

                const Int64 AL = oa.allocationLimit();
                oa.setAllocationLimit(-1);

                ASSERT(checkIntegrity(X, X.size()));
                mX = Y;

                oa.setAllocationLimit(AL);

                const Int64 BB = oa.numBlocksTotal();
                const Int64 B  = oa.numBlocksInUse();

                mX.assign(LIST);

                const Int64 AA = oa.numBlocksTotal();
                const Int64 A  = oa.numBlocksInUse();

                ASSERTV(k_NO_EXCEPT || AA - BB == numThrows);
                ASSERT(A - B == deltaBlocks(LENGTH - DST_LENGTH));

                ASSERTV(EXPECTED, X, EXPECTED == X);
                ASSERT(checkIntegrity(X, LENGTH));

                ASSERT(xoa == X.get_allocator());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        ASSERTV(SPEC,
                bsls::NameOf<TYPE>(),
                DD,
                defaultAllocator_p->numBlocksTotal(),
                DD == defaultAllocator_p->numBlocksTotal());
    }
#else
    if (verbose) printf("*NOT* TESTING INITIALIZER LIST - NOT C++11\n"
                        "==========================================\n");
#endif
}

template <class TYPE, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver2<TYPE, ALLOC>::
                       test31_propagate_on_container_move_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   TYPE,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG>
                                      StdAlloc;

    typedef bsl::list<TYPE, StdAlloc> Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    // Create control and source objects.
    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const ISPEC   = SPECS[ti];

        TestValues IVALUES(ISPEC);

        bslma::TestAllocator oas("source", veryVeryVeryVerbose);
        bslma::TestAllocator oat("target", veryVeryVeryVerbose);

        StdAlloc mas(&oas);
        StdAlloc mat(&oat);

        StdAlloc scratch(&da);

        const Obj W(IVALUES.begin(), IVALUES.end(), scratch);  // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            {
                IVALUES.resetIterators();

                Obj        mY(IVALUES.begin(), IVALUES.end(), mas);
                const Obj& Y = mY;

                if (veryVerbose) { T_ P_(ISPEC) P_(Y) P(W) }

                Obj        mX(JVALUES.begin(), JVALUES.end(), mat);
                const Obj& X = mX;

                bslma::TestAllocatorMonitor oasm(&oas);
                bslma::TestAllocatorMonitor oatm(&oat);

                Obj *mR = &(mX = MoveUtil::move(mY));

                ASSERTV(ISPEC, JSPEC,  W,   X,  W == X);
                ASSERTV(ISPEC, JSPEC, mR, &mX, mR == &mX);

                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                       !PROPAGATE == (mat == X.get_allocator()));
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                        PROPAGATE == (mas == X.get_allocator()));

                ASSERTV(ISPEC, JSPEC, mas == Y.get_allocator());

                if (PROPAGATE) {
                    ASSERTV(ISPEC, JSPEC, 0 == oat.numBlocksInUse());
                }
                else {
                    ASSERTV(ISPEC, JSPEC, oasm.isInUseSame());
                }
            }
            ASSERTV(ISPEC, 0 == oas.numBlocksInUse());
            ASSERTV(ISPEC, 0 == oat.numBlocksInUse());
        }
    }
    ASSERTV(0 == da.numBlocksInUse());
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::test31_propagate_on_container_move_assignment()
{
    // ------------------------------------------------------------------------
    // MOVE-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_move_assignment' trait is 'false', the
    //:   allocator used by the target object remains unchanged (i.e., the
    //:   source object's allocator is *not* propagated).
    //:
    //: 2 If the 'propagate_on_container_move_assignment' trait is 'true', the
    //:   allocator used by the target object is updated to be a copy of that
    //:   used by the source object (i.e., the source object's allocator *is*
    //:   propagated).
    //:
    //: 3 The allocator used by the source object remains unchanged whether or
    //;   not it is propagated to the target object.
    //:
    //: 4 If the allocator is propagated from the source object to the target
    //:   object, all memory allocated from the target object's original
    //:   allocator is released.
    //:
    //: 5 The effect of the 'propagate_on_container_move_assignment' trait is
    //:   independent of the other three allocator propagation traits.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_move_assignment' property configured to
    //:   'false'.  In two successive iterations of P-3, first configure the
    //:   three properties not under test to be 'false', then configure them
    //:   all to be 'true'.
    //:
    //: 3 For each value '(x, y)' in the cross product S x S:  (C-1)
    //:
    //:   1 Initialize an object 'X' from 'x' using one of the allocators from
    //:     P-2.
    //:
    //:   2 Initialize two objects from 'y', a control object 'W' using a
    //:     scratch allocator and an object 'Y' using the other allocator from
    //:     P-2.
    //:
    //:   3 Move-assign 'Y' to 'X' and use 'operator==' to verify that 'X'
    //:     subsequently has the same value as 'W'.
    //:
    //:   4 Use the 'get_allocator' method to verify that the allocator of 'Y'
    //:     is *not* propagated to 'X' and that the allocator used by 'Y'
    //:     remains unchanged.  (C-1)
    //:
    //: 4 Repeat P-2..3 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocator of 'Y' *is*
    //:   propagated to 'X'.  Also verify that all memory is released to the
    //:   allocator that was in use by 'X' prior to the assignment.  (C-2..5)
    //
    // Testing:
    //   propagate_on_container_move_assignment
    // ------------------------------------------------------------------------

    if (verbose) printf("\nMOVE-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION"
                        "\n===============================================\n");

    if (verbose)
        printf("\n'propagate_on_container_move_assignment::value == false'\n");

    test31_propagate_on_container_move_assignment_dispatch<false, false>();
    test31_propagate_on_container_move_assignment_dispatch<false, true>();

    if (verbose)
        printf("\n'propagate_on_container_move_assignment::value == true'\n");

    test31_propagate_on_container_move_assignment_dispatch<true, false>();
    test31_propagate_on_container_move_assignment_dispatch<true, true>();
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test31_moveAssign_noexcept()
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
    //   list& operator=(bslmf::MovableRef<list> rhs);
    // ------------------------------------------------------------------------

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    bsl::list<TYPE, ALLOC> a, b;

    const bool isNoexcept = AllocTraits::is_always_equal::value;
    ASSERT(isNoexcept ==
        BSLS_KEYWORD_NOEXCEPT_OPERATOR(a = bslmf::MovableRefUtil::move(b)));
#endif
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test31_moveAssign()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE ASSIGN
    //
    // Concerns:
    //: 1 The destination object's value is the same as that of the source
    //:   object (relying on the equality operator) and created with the
    //:   correct allocator.
    //:
    //: 2 If the allocators match, the assignment is a swap.
    //:
    //: 3 If the allocators don't match, the assignment is a copy and the value
    //:   of the 'rhs' is unaffected (this is not a guaranteed property of the
    //:   component -- it's just that for the types we test, a 'move' between
    //:   objects that don't share the same allocator is a copy).
    //:
    //: 4 Subsequent changes to the source object have no effect on the
    //:   assigned object.
    //:
    //: 5 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 6 If 'allocator_traits<Allocator>::is_always_equal::value' is
    //:   true, the move assignment operator is 'noexcept(true)'.
    //
    // Plan:
    //: 1 Specify a set S of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used in the
    //:   following tests.
    //:
    //: 2 For concerns 1 - 3, for each value in S, initialize objects w and v.
    //:   Copy object w to x and v to y.  Move assign y to x.
    //:   o If the allocators match, verify it was a swap.
    //:   o If the allocators don't match, verify it was a copy.
    //:
    //: 3 For concern 4, clear x and observe y is unchanged.
    //:
    //: 4 To address concern 5, observe the default allocator before and after
    //:   the whole test and observe that it is never used (after the first
    //:   call to 'checkIntegrity'.
    //:
    //: 5 To address concern 6, pass allocators with both 'is_always_equal'
    //:   values (true & false).
    //
    // Testing:
    //   list& operator=(list&& orig);
    // ------------------------------------------------------------------------

    if (verbose) printf("Test move assign, TYPE: %s, k_SCOPED_ALLOC: %d\n",
                                  bsls::NameOf<TYPE>().name(), k_SCOPED_ALLOC);

    if (k_SCOPED_ALLOC) {
        Obj X;
        (void) checkIntegrity(X, 0);    // 'checkIntegrity' uses the default
                                        // allocator the first time it is
                                        // called.
    }
    const Int64 DD = defaultAllocator_p->numBlocksTotal();

    bslma::TestAllocator oaa("objectA", veryVeryVeryVerbose);
    const ALLOC          xoa(&oaa);

    bslma::TestAllocator oab("objectB", veryVeryVeryVerbose);
    const ALLOC          xob(&oab);

    ASSERT(xoa != xob);

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
        "DEAB",
        "EABCD",
        "ABCDEAB",
        "BCDEABCD",
        "CDEABCDEA",
        "DEABCDEABCDE",
        "EABCDEABCDEAB",
        "ABCDEABCDEABCD",
    };
    enum { NUM_SPECS = sizeof(SPECS) / sizeof SPECS[0] };

    IntPtr oldXLen = -1;
    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char   *XSPEC   = SPECS[ti];
        const IntPtr  XLENGTH = std::strlen(XSPEC);

        ASSERTV(ti, XSPEC, oldXLen, oldXLen < XLENGTH); // strictly increasing
        oldXLen = XLENGTH;

        if (veryVerbose) {
            printf("From an object of length " TD ": XSPEC: %s\n",
                                                           XLENGTH, XSPEC);
        }

        // Create control object 'W'.
        Obj mW(xoa);  const Obj& W = gg(&mW, XSPEC);

        ASSERTV(ti, CMPINT(XLENGTH, ==, W.size())); // same lengths

        if (veryVerbose) { printf("\tControl Obj: "); P(W); }

        IntPtr oldYLen = -1;
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char   *YSPEC   = SPECS[tj];
            const IntPtr  YLENGTH = std::strlen(YSPEC);

            if (veryVerbose) { P_(tj); P(YSPEC); }

            ASSERTV(tj, YSPEC, SPECS[tj], oldYLen, YLENGTH, oldYLen < YLENGTH);
            oldYLen = YLENGTH;

            if (veryVeryVerbose) {
                printf("To an object of length " TD ": YSPEC: %s\n",
                                                               YLENGTH, YSPEC);
            }

            Obj mV(xoa);  const Obj& V = gg(&mV, YSPEC);

            {
                if (veryVerbose) { printf("\tMatching allocators.\n"); }

                Obj mX(xoa);  const Obj& X = gg(&mX, XSPEC);
                ASSERT(X == W);

                Obj mY(xoa);  const Obj& Y = gg(&mY, YSPEC);
                ASSERT(Y == V);

                ASSERT((ti == tj) == (X == Y));

                if (veryVerbose) { printf("\tObj : "); P(Y); }

                const Int64 BB = oaa.numBlocksTotal();

                // should be just a swap, no allocators, no moves of elements

                mY = MoveUtil::move(mX);

                const Int64 AA = oaa.numBlocksTotal();

                ASSERT(AA == BB);

                ASSERT(Y == W);
                ASSERT(X == V);

                ASSERTV(XSPEC, checkIntegrity(X, YLENGTH));

                mX.clear();

                ASSERT(Y == W);

                ASSERT(0 == numMovedFrom(X));

                ASSERTV(XSPEC, checkIntegrity(X, 0));
                ASSERTV(YSPEC, checkIntegrity(Y, XLENGTH));
                ASSERTV(YSPEC, Y.get_allocator() == xoa);
                ASSERTV(XSPEC, X.get_allocator() == xoa);
            } // matching allocators

            {
                if (veryVerbose) { printf("\tNon-matching allocators.\n"); }

                Obj mX(xob);  const Obj& X = gg(&mX, XSPEC);
                ASSERT(X == W);

                Obj mY(xoa);  const Obj& Y = gg(&mY, YSPEC);
                ASSERT(Y == V);

                ASSERT((ti == tj) == (X == Y));

                if (veryVerbose) { printf("\tObj : "); P(Y); }

                const Int64 BB = oaa.numBlocksTotal();
                const Int64 B  = oaa.numBlocksInUse();

                mY = MoveUtil::move(mX);

                const Int64 AA = oaa.numBlocksTotal();
                const Int64 A  = oaa.numBlocksInUse();

                const IntPtr wSize = W.size(), vSize = V.size();

                ASSERT(AA >= BB + std::max<Int64>(deltaBlocks(wSize - vSize),
                                                  0));
                ASSERT(A  == B  + deltaBlocks(wSize - vSize));

                ASSERT(Y == W);

                bool expectMove = IsMoveAware::value && !k_IS_WELL_BEHAVED;

                ASSERTV((int) Y.size(), numMovedInto(Y),
                             !expectMove || (int) Y.size() == numMovedInto(Y));
                ASSERTV((int) X.size(), numMovedFrom(X),
                             !expectMove || (int) X.size() == numMovedFrom(X));

                ASSERTV(XSPEC, checkIntegrity(X, XLENGTH));
                mX.clear();
                ASSERT(Y == W);

                ASSERT(0 == numMovedFrom(X));

                ASSERTV(XSPEC, checkIntegrity(X, 0));
                ASSERTV(YSPEC, checkIntegrity(Y, XLENGTH));
                ASSERTV(YSPEC, Y.get_allocator() == xoa);
                ASSERTV(XSPEC, X.get_allocator() == xob);
            } // non-matching allocators

            {
                if (veryVerbose) {
                    printf("\tNon-matching allocators + exceptions.\n");
                }

                Obj mY(xoa);  const Obj& Y = gg(&mY, YSPEC);
                ASSERT(Y == V);

                if (veryVerbose) { printf("\tObj : "); P(Y); }

                int numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oaa) {
                    const Int64 AL = oaa.allocationLimit();
                    oaa.setAllocationLimit(-1);

                    ++numThrows;

                    Obj mX(xob);   const Obj& X = gg(&mX, XSPEC);
                    ASSERT(X == W);
                    ASSERT(0 == numMovedFrom(X));

                    // Y might be changed, but should be valid and its length
                    // should not shrink (if it shrinks at all) until after all
                    // allocators are done.

                    ASSERT(Y.size() >= V.size());
                    for (size_t ty = 0; ty < Y.size(); ++ty) {
                        const TYPE& E = nthElem(Y, ty);

                        ASSERTV(ty, Y.size(), V.size(), W.size(),
                                (ty < V.size() && E == nthElem(V, ty)) ||
                                       (ty < W.size() && E == nthElem(W, ty)));
                    }
                    ASSERTV(YSPEC, checkIntegrity(Y, Y.size()));

                    mY.clear();
                    gg(&mY, YSPEC);
                    ASSERT(Y == V);

                    ASSERT((ti == tj) == (X == Y));

                    oaa.setAllocationLimit(AL);

                    const Int64 BB = oaa.numBlocksTotal();
                    const Int64 B  = oaa.numBlocksInUse();

                    mY = MoveUtil::move(mX);

                    const Int64 AA = oaa.numBlocksTotal();
                    const Int64 A  = oaa.numBlocksInUse();

                    const IntPtr wSize = W.size(), vSize = V.size();

                    ASSERT(AA >=
                             BB + std::max<Int64>(deltaBlocks(wSize - vSize),
                                                  0));
                    ASSERT(A  == B + deltaBlocks(wSize - vSize));
                    ASSERT(k_NO_EXCEPT || numThrows == AA - BB);

                    ASSERT(Y == W);

                    bool expectMove = IsMoveAware::value && !k_IS_WELL_BEHAVED;

                    ASSERTV((int) Y.size(), numMovedInto(Y),
                             !expectMove || (int) Y.size() == numMovedInto(Y));
                    ASSERTV((int) X.size(), numMovedFrom(X),
                             !expectMove || (int) X.size() == numMovedFrom(X));


                    ASSERTV(XSPEC, checkIntegrity(X, XLENGTH));
                    ASSERTV(YSPEC, checkIntegrity(Y, XLENGTH));
                    ASSERTV(YSPEC, Y.get_allocator() == xoa);
                    ASSERTV(XSPEC, X.get_allocator() == xob);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            } // non-matching allocators
        } // Y
    } // X

    ASSERTV(bsls::NameOf<TYPE>(), defaultAllocator_p->numBlocksTotal() == DD);

    // Test noexcept specification of the move assignment operator.
    TestDriver2<TYPE, ALLOC>::test31_moveAssign_noexcept();
    // is_always_equal == true
    typedef StatelessAllocator<TYPE> StatelessAlloc;
    ASSERT(bsl::allocator_traits<StatelessAlloc>::is_always_equal::value);
    TestDriver2<TYPE, StatelessAlloc>::test31_moveAssign_noexcept();
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test30_moveCtor()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE CONSTRUCTOR
    //
    // Concerns:
    //: 1 The new object's value is the same as the initial value of the
    //:   original object (relying on the equality operator) and created with
    //:   the correct allocator.
    //:
    //: 2 If no allocator is passed, or if the allocators match, the operation
    //:   is a swap.
    //:
    //: 3 if an allocator that does not match the allocator of the source
    //:   object, the operation is a copy (this is not guaranteed by the
    //:   component -- individual elements are moved, but for all the types we
    //:   test, a move with different allocators is a copy.
    //:
    //: 4 Subsequent changes of the source object have no effect on the
    //:   copy-constructed object.
    //:
    //: 5 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 6 The function is exception neutral w.r.t. memory allocation.
    //:
    //: 7 An object copied from an rvalue with no allocator, or with a matching
    //:   allocator specified, will leave the copied object in a valid,
    //:   default-constructed state.
    //:   1 No allocator specified to c'tor.
    //:   2 Allocator specified to c'tor.
    //:
    //: 8 An object copied from an rvalue with a non-matching allocator
    //:   supplied will not modify the rvalue.
    //
    // Plan:
    //: 1 Specify a set S of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used in the
    //:   following tests.
    //:
    //: 2 Default-construct a const object D.
    //:
    //: 3 For concerns 1 - 3, for each value in S, initialize objects w and x,
    //:   move construct y from x passing no allocator, and passing an
    //:   allocator that matches 'x's allocator, and in both cases use
    //:   'operator==' to verify that y subsequently has the same value as w,
    //:   and that 'D == x'.
    //:
    //: 4 Modify x and observe y is unchanged.
    //:
    //: 5 Construct an object y from x where the allocators don't match, and
    //:   observe afterward that both y and x equal w.
    //
    // Testing:
    //   list(list&& orig, const A& = A());
    // ------------------------------------------------------------------------

    if (verbose) printf("Test move c'tor, TYPE: %s\n",
                        bsls::NameOf<TYPE>().name());

    if (k_SCOPED_ALLOC) {
        Obj X;
        (void) checkIntegrity(X, 0);    // 'checkIntegrity' uses the default
                                        // allocator the first time it is
                                        // called.
    }
    const Int64 DD = defaultAllocator_p->numBlocksTotal();

    bslma::TestAllocator oaa("objectA", veryVeryVeryVerbose);
    const ALLOC          xoa(&oaa);

    bslma::TestAllocator oab("objectB", veryVeryVeryVerbose);
    const ALLOC          xob(&oab);

    ASSERT(xoa != xob);

    if (verbose)
        printf("Testing parameters: k_SCOPED_ALLOC = %d.\n", k_SCOPED_ALLOC);
    {
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
        };
        enum { NUM_SPECS = sizeof(SPECS) / sizeof SPECS[0] };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = std::strlen(SPEC);

            if (veryVerbose) {
                printf("For an object of length " ZU ": SPEC: %s\n",
                                                                 LENGTH, SPEC);
            }

            ASSERTV(SPEC, oldLen < (int)LENGTH);    // strictly increasing

            oldLen = static_cast<int>(LENGTH);

            // Create control object w & d
            Obj       mW(xoa);  const Obj& W = gg(&mW, SPEC);
            const Obj mD(xoa);  const Obj& D = mD;  // default constructed

            ASSERTV(ti, LENGTH == W.size());        // same lengths

            if (veryVerbose) { printf("\tControl Obj: "); P(W); }
            {   // Testing concerns 1 & 4
                if (veryVerbose) { printf("\tNo alloc passed.\n"); }

                Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                ASSERTV(SPEC, X == W);

                const Int64 BB = oaa.numBlocksTotal();

                Obj mY(MoveUtil::move(mX));  const Obj& Y = mY;

                if (veryVerbose) {
                    printf("\tObj : "); P(Y);
                }

                // one allocation, just create one sentinel and swap

                const Int64 AA = oaa.numBlocksTotal();

                ASSERTV(SPEC, AA - BB, BB + 1 == AA);
                ASSERTV(SPEC, checkIntegrity(Y, LENGTH));
                ASSERTV(SPEC, W == Y);
                ASSERTV(SPEC, checkIntegrity(X, 0));
                ASSERTV(SPEC, D == X);
                ASSERTV(SPEC, 0 == LENGTH || Y != X);
                ASSERTV(SPEC, Y.get_allocator() == xoa);
                ASSERTV(SPEC, X.get_allocator() == xoa);

                primaryManipulator(&mX, VG);

                ASSERTV(SPEC, checkIntegrity(Y, LENGTH));
                ASSERTV(SPEC, W == Y);
                ASSERTV(SPEC, checkIntegrity(X, 1));
                ASSERTV(SPEC, D != X);
                ASSERTV(SPEC, Y != X);
                ASSERTV(SPEC, Y.get_allocator() == xoa);
                ASSERTV(SPEC, X.get_allocator() == xoa);

                Obj mXG(xoa);  const Obj& XG = mXG;

                primaryManipulator(&mXG, VG);

                ASSERTV(SPEC, XG == X);
                ASSERTV(SPEC, checkIntegrity(X,  1));
                ASSERTV(SPEC, checkIntegrity(XG, 1));

                primaryManipulator(&mY, VH);

                ASSERTV(SPEC, checkIntegrity(Y, LENGTH + 1));
                ASSERTV(SPEC, W != Y);
                ASSERTV(SPEC, checkIntegrity(X, 1));
                ASSERTV(SPEC, Y != X);
                ASSERTV(SPEC, XG == X);
                ASSERTV(SPEC, Y.get_allocator() == xoa);
                ASSERTV(SPEC, X.get_allocator() == xoa);
            }

            {   // Testing concerns 1 & 4
                if (veryVerbose) { printf("\tMatching alloc passed.\n"); }

                Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                ASSERTV(SPEC, X == W);

                Obj mY(MoveUtil::move(mX), xoa);  const Obj& Y = mY;

                if (veryVerbose) {
                    printf("\tObj : "); P(Y);
                }

                ASSERTV(SPEC, checkIntegrity(Y, LENGTH));
                ASSERTV(SPEC, W == Y);
                ASSERTV(SPEC, checkIntegrity(X, 0));
                ASSERTV(SPEC, D == X);
                ASSERTV(SPEC, 0 == LENGTH || Y != X);
                ASSERTV(SPEC, Y.get_allocator() == xoa);
                ASSERTV(SPEC, X.get_allocator() == xoa);

                primaryManipulator(&mX, VG);

                ASSERTV(SPEC, checkIntegrity(Y, LENGTH));
                ASSERTV(SPEC, W == Y);
                ASSERTV(SPEC, checkIntegrity(X, 1));
                ASSERTV(SPEC, D != X);
                ASSERTV(SPEC, Y != X);
                ASSERTV(SPEC, Y.get_allocator() == xoa);
                ASSERTV(SPEC, X.get_allocator() == xoa);

                Obj mXG(xoa);  const Obj& XG = mXG;

                primaryManipulator(&mXG, VG);

                ASSERTV(SPEC, XG == X);
                ASSERTV(SPEC, checkIntegrity(X,  1));
                ASSERTV(SPEC, checkIntegrity(XG, 1));

                primaryManipulator(&mY, VH);

                ASSERTV(SPEC, checkIntegrity(Y, LENGTH + 1));
                ASSERTV(SPEC, W != Y);
                ASSERTV(SPEC, checkIntegrity(X, 1));
                ASSERTV(SPEC, Y != X);
                ASSERTV(SPEC, XG == X);
                ASSERTV(SPEC, Y.get_allocator() == xoa);
                ASSERTV(SPEC, X.get_allocator() == xoa);
            }

            {   // Testing concern 1.
                if (veryVerbose) { printf("\tNon-matching alloc passed.\n"); }

                Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                ASSERTV(SPEC, X == W);

                Obj mY(MoveUtil::move(mX), xob);  const Obj& Y = mY;

                if (veryVerbose) { printf("\tObj : "); P(Y); }

                ASSERTV(SPEC, checkIntegrity(Y, LENGTH));
                ASSERTV(SPEC, checkIntegrity(X, LENGTH));
                ASSERTV(SPEC, W == Y);
                ASSERTV(SPEC, 0 == LENGTH || D != X);
                ASSERTV(SPEC, X.get_allocator() == xoa);
                ASSERTV(SPEC, Y.get_allocator() == xob);

                bool expectMove = IsMoveAware::value && !k_IS_WELL_BEHAVED;

                ASSERTV((int) Y.size(), numMovedInto(Y),
                             !expectMove || (int) Y.size() == numMovedInto(Y));
                ASSERTV((int) X.size(), numMovedFrom(X),
                             !expectMove || (int) X.size() == numMovedFrom(X));

                primaryManipulator(&mX, VG);

                ASSERTV(SPEC, checkIntegrity(Y, LENGTH));
                ASSERTV(SPEC, checkIntegrity(X, LENGTH + 1));
                ASSERTV(SPEC, W == Y);
                ASSERTV(SPEC, W != X);
                ASSERTV(SPEC, Y != X);
                ASSERTV(SPEC, D != X);
                ASSERTV(SPEC, X.get_allocator() == xoa);
                ASSERTV(SPEC, Y.get_allocator() == xob);
            }

#ifdef BDE_BUILD_TARGET_EXC
            {
                if (veryVerbose) {
                    printf("\tNo alloc passed, exceptions.\n");
                }

                Obj mX(xoa);

                const Int64 B = oaa.numBlocksInUse();

                const Obj& X = gg(&mX, SPEC);

                ASSERTV(SPEC, X == W);

                int numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oaa) {
                    ++numThrows;

                    ASSERTV(SPEC, W == X);

                    const Int64 BB = oaa.numBlocksTotal();

                    // one allocation, just create one sentinel and swap

                    Obj mY(MoveUtil::move(mX));  const Obj& Y = mY;

                    if (veryVerbose) { printf("\tObj : "); P(Y); }

                    const Int64 AA = oaa.numBlocksTotal();

                    ASSERT(BB + 1 == AA);
                    ASSERT(k_NO_EXCEPT || 1 == numThrows);

                    ASSERTV(SPEC, checkIntegrity(Y, LENGTH));
                    ASSERTV(SPEC, W == Y);
                    ASSERTV(SPEC, checkIntegrity(X, 0));
                    ASSERTV(SPEC, D == X);
                    ASSERTV(SPEC, 0 == LENGTH || Y != X);
                    ASSERTV(SPEC, Y.get_allocator() == xoa);
                    ASSERTV(SPEC, X.get_allocator() == xoa);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERT(k_NO_EXCEPT || 1 == numThrows);

                const Int64  A = oaa.numBlocksInUse();

                ASSERTV(SPEC, A == B);
            }

            {
                if (veryVerbose) {
                    printf("\tMatching alloc passed, exceptions.\n");
                }

                Obj mX(xoa);

                const Int64 B = oaa.numBlocksInUse();

                const Obj& X = gg(&mX, SPEC);

                ASSERTV(SPEC, X == W);

                int numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oaa) {
                    ++numThrows;

                    ASSERTV(SPEC, W == X);

                    const Int64 BB = oaa.numBlocksTotal();

                    // one allocation, just create one sentinel and swap

                    Obj mY(MoveUtil::move(mX), xoa);  const Obj& Y = mY;

                    if (veryVerbose) { printf("\tObj : "); P(Y); }

                    const Int64 AA = oaa.numBlocksTotal();

                    ASSERT(BB + 1 == AA);
                    ASSERT(k_NO_EXCEPT || 1 == numThrows);

                    ASSERTV(SPEC, checkIntegrity(Y, LENGTH));
                    ASSERTV(SPEC, W == Y);
                    ASSERTV(SPEC, checkIntegrity(X, 0));
                    ASSERTV(SPEC, D == X);
                    ASSERTV(SPEC, 0 == LENGTH || Y != X);
                    ASSERTV(SPEC, Y.get_allocator() == xoa);
                    ASSERTV(SPEC, X.get_allocator() == xoa);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERT(k_NO_EXCEPT || 1 == numThrows);

                const Int64  A = oaa.numBlocksInUse();

                ASSERTV(SPEC, A == B);
            }

            {
                if (veryVerbose) {
                    printf("\tNon-Matching alloc passed, exceptions.\n");
                }

                Obj mX(xob);  const Obj& X = gg(&mX, SPEC);

                ASSERTV(SPEC, X == W);

                int numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oaa) {
                    ++numThrows;

                    // No strong exception guarantee, just that X must be in a
                    // valid state after throw and Y must be in a valid state
                    // upon construction.

                    ASSERTV(SPEC, checkIntegrity(X, LENGTH));

                    // Note that some nodes in X might be marked 'moved from',
                    // so we have to wipe clean any 'moved from' state to
                    // silence asserts.

                    const Int64 AL = oaa.allocationLimit();
                    oaa.setAllocationLimit(-1);

                    mX.clear();
                    gg(&mX, SPEC);

                    ASSERTV(SPEC, X == W);

                    oaa.setAllocationLimit(AL);

                    Obj mY(MoveUtil::move(mX), xoa);  const Obj& Y = mY;

                    ASSERTV(SPEC, checkIntegrity(X, LENGTH));
                    ASSERTV(SPEC, checkIntegrity(Y, LENGTH));
                    ASSERTV(SPEC, Y == W);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERT(k_NO_EXCEPT || CMPINT(numThrows, >=, LENGTH));
            }
#endif // BDE_BUILD_TARGET_EXC
        }
    }

    ASSERTV(bsls::NameOf<TYPE>(), defaultAllocator_p->numBlocksTotal() == DD);
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test29_moveInsert()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE INSERTION
    //
    // Concerns
    //: 1 That the resulting list value is correct.
    //:
    //: 2 That the 'insert' return (if any) value is a valid iterator to the
    //:   first inserted element or to the insertion position if no elements
    //:   are inserted.
    //:
    //: 3 That insertion of one element has the strong exception guarantee.
    //:
    //: 4 That insertion is exception neutral w.r.t. memory allocation.
    //:
    //: 5 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 6 That inserting a 'const T& value' that is a reference to an element
    //:   of the list does not suffer from aliasing problems.
    //:
    //: 7 That no iterators are invalidated by the insertion.
    //:
    //: 8 That inserting 'n' copies of value 'v' selects the correct overload
    //:   when 'n' and 'v' are identical arithmetic types (i.e., the
    //:   iterator-range overload is not selected).
    //:
    //: 9 That inserting 'n' copies of value 'v' selects the correct overload
    //:   when 'v' is a pointer type and 'n' is a null pointer literal ,'0'.
    //:   (i.e., the iterator-range overload is not selected).
    //
    // Plan:
    //: 1 Create objects of various sizes and insert a distinct value one or
    //:   more times into each possible position.
    //:
    //: 2 For concerns 1, 2 & 5, verify that the return value and modified list
    //:   are as expected.
    //:
    //: 3 For concerns 3 & 4 perform the test using the exception-testing
    //:   infrastructure and verify the value and memory changes.
    //:
    //: 4 For concern 6, we select the value to insert from the middle of the
    //:   list, thus testing insertion before, at, and after the aliased
    //:   element.
    //:
    //: 5 For concern 7, save copies of the iterators before and after the
    //:   insertion point and verify that they point to the same (valid)
    //:   elements after the insertion by iterating to the same point in the
    //:   resulting list and comparing the new iterators to the old ones.
    //:
    //: 6 For concerns 8 and 9, insert 2 elements of integral or pointer types
    //:   into lists and verify that it compiles and that the resultant list
    //:   contains the expected values.
    //
    // Testing:
    //   iterator insert(const_iterator pos, T&& value);
    //   void push_back(T&& value);
    //   void push_front(T&& value);
    //   Concern: All emplace methods handle rvalues.
    // ------------------------------------------------------------------------

    if (verbose) printf("TESTING MOVE INSERTION\n"
                        "======================\n");

    if (verbose) printf("TYPE: %s\n", bsls::NameOf<TYPE>().name());

    bslma::TestAllocator oa(veryVeryVeryVerbose);
    const ALLOC          xoa(&oa);

    const int           MAX_LEN    = 15;

    // Operations to test
    enum {
        TEST_BEGIN,
        TEST_INSERT = TEST_BEGIN,     // insert(pos, value);
        TEST_EMPLACE,                 // emplace(pos, value);
        TEST_EMPLACE_BACK,            // emplace_back(value);
        TEST_EMPLACE_FRONT,           // emplace_front(value)
        TEST_PUSH_BACK,               // push_back(value);
        TEST_PUSH_FRONT,              // push_front(value);
        TEST_END
    };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // container spec
    } DATA[] = {
        //line  spec                  length
        //----  -----------------     ------
        { L_,   ""                 }, // 0
        { L_,   "A"                }, // 1
        { L_,   "AB"               }, // 2
        { L_,   "ABC"              }, // 3
        { L_,   "ABCD"             }, // 4
        { L_,   "ABCDA"            }, // 5
        { L_,   "ABCDABCDABCDABC"  }, // 15
    };
    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    Obj mSrc(xoa);  const Obj& SRC = mSrc;

    primaryManipulator(&mSrc, VH);

    const TYPE& NEW_ELEM_VALUE = SRC.front();

    ASSERTV(SRC.size(), 1 == SRC.size());

    const Int64 startBlocksInUse = oa.numBlocksInUse();
    const Int64 delta            = IsMoveAware::value ? 1 : deltaBlocks(1);

    // Iterate through the operations
    for (int op = TEST_BEGIN; op < TEST_END; ++op) {
        for (unsigned ti = 0; ti < NUM_DATA; ++ti) {
            const int       LINE   = DATA[ti].d_lineNum;
            const char     *SPEC   = DATA[ti].d_spec_p;
            const unsigned  LENGTH = static_cast<unsigned>(std::strlen(SPEC));

            ASSERTV(LENGTH, LENGTH <= MAX_LEN);

            for (unsigned posidx = 0; posidx <= LENGTH; ++posidx) {
                if ((TEST_PUSH_BACK == op || TEST_EMPLACE_BACK == op)
                                                         && LENGTH != posidx) {
                    continue;  // Can push_back only at end
                }
                else if ((TEST_PUSH_FRONT == op || TEST_EMPLACE_FRONT == op)
                                                              && 0 != posidx) {
                    continue;  // Can push_front only at beginning
                }

                Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);
                Obj mY;       const Obj& Y = gg(&mY, SPEC);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const Int64 AL = oa.allocationLimit();
                    oa.setAllocationLimit(-1);

                    ASSERT(X == Y && &X != &Y);  // Strong exception guarantee.

                    // Choose a value to insert that is deliberately aliasing a
                    // list element.

                    mSrc.resize(1);
                    primaryManipulator(&mSrc, VH);
                    ASSERT(SRC.front() == SRC.back() &&
                               bsls::Util::addressOf(SRC.front()) !=
                                            bsls::Util::addressOf(SRC.back()));
                    TYPE& NEW_ELEM_MOV = mSrc.back();

                    ASSERT(NEW_ELEM_VALUE == NEW_ELEM_MOV &&
                               bsls::Util::addressOf(NEW_ELEM_VALUE) !=
                                          bsls::Util::addressOf(NEW_ELEM_MOV));
                    ASSERT(!IsMoveAware::value ||
                                           !bsltf::getMovedFrom(NEW_ELEM_MOV));
                    ASSERTV(SRC.size(), 2 == SRC.size());

                    // Save original iterators (including end iterator).  C++0x
                    // allows insertion using const_iterator

                    const_iterator orig_iters[MAX_LEN + 1];
                    const_iterator it = X.begin();
                    for (unsigned i = 0; i < LENGTH + 1; ++i, ++it) {
                        orig_iters[i] = it;
                    }

                    oa.setAllocationLimit(AL);

                    unsigned n = 0;

                    // C++0x allows insertion using const_iterator

                    const_iterator   pos = orig_iters[posidx];
                    iterator         ret;
                    ExceptionProctor proctor(&mX, LINE);

                    const Int64 B = oa.numBlocksInUse();

                    switch (op) {
                      case TEST_INSERT: {
                        ret = mX.insert(pos, MoveUtil::move(NEW_ELEM_MOV));
                        n = 1;
                      } break;
                      case TEST_EMPLACE: {
                        ret = mX.emplace(pos, MoveUtil::move(NEW_ELEM_MOV));
                        n = 1;
                      } break;
                      case TEST_EMPLACE_BACK: {
                        mX.emplace_back(MoveUtil::move(NEW_ELEM_MOV));
                        ret = --mX.end();
                        n = 1;
                      } break;
                      case TEST_EMPLACE_FRONT: {
                        mX.emplace_front(MoveUtil::move(NEW_ELEM_MOV));
                        ret = mX.begin();
                        n = 1;
                      } break;
                      case TEST_PUSH_BACK: {
                        mX.push_back(MoveUtil::move(NEW_ELEM_MOV));
                        ret = --mX.end();
                        n = 1;
                      } break;
                      case TEST_PUSH_FRONT: {
                        mX.push_front(MoveUtil::move(NEW_ELEM_MOV));
                        ret = mX.begin();
                        n = 1;
                      } break;
                      default: {
                        ASSERT(0);
                      } break;
                    } // end switch
                    proctor.release();

                    // If got here, then there was no exception

                    const Int64 A = oa.numBlocksInUse();

                    ASSERTV(bsls::NameOf<TYPE>(),
                            !IsMoveAware::value || MoveState::e_MOVED ==
                                            bsltf::getMovedFrom(NEW_ELEM_MOV));
                    ASSERT(bsls::Util::addressOf(mSrc.back()) ==
                                          bsls::Util::addressOf(NEW_ELEM_MOV));
                    mSrc.pop_back();
                    ASSERTV(SRC.size(), 1 == SRC.size());
                    ASSERT(SRC.front() == SRC.back() &&
                                bsls::Util::addressOf(SRC.front()) ==
                                            bsls::Util::addressOf(SRC.back()));
                    ASSERT(*ret == NEW_ELEM_VALUE);

                    // Test important values
                    ASSERTV(LINE, op, posidx, checkIntegrity(X, LENGTH + n));
                    ASSERTV(LINE, op, posidx, LENGTH + n == X.size());
                    ASSERTV(LINE, op, posidx, B, delta, A, B + delta == A);

                    // Test return value from 'insert'
                    ASSERTV(LINE, op, posidx,
                            posDistance(mX.begin(), ret) == posidx);

                    const_iterator cit = X.begin();
                    const_iterator yi = Y.begin();
                    for (unsigned i = 0; i < X.size(); ++i, ++cit) {
                        if (i < posidx) {
                            // Test that part before insertion is unchanged
                            ASSERTV(LINE, op, posidx, i, *yi++ == *cit);
                            ASSERTV(LINE, op, posidx, i, orig_iters[i] == cit);
                        }
                        else if (i < posidx + n) {
                            // Test inserted values
                            ASSERTV(LINE, op, posidx, i,
                                    NEW_ELEM_VALUE == *cit);
                        }
                        else {
                            // Test that part after insertion is unchanged
                            ASSERTV(LINE, op, posidx, i, *yi++ == *cit);
                            ASSERTV(LINE, op, posidx, i,
                                    orig_iters[i - n] == cit);
                        }
                    }
                    // Test end iterator
                    ASSERTV(LINE, op, posidx, X.end() == cit);
                    ASSERTV(LINE, op, posidx, orig_iters[LENGTH] == cit);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            } // end for (posidx)

            ASSERTV(LINE, op, startBlocksInUse, oa.numBlocksInUse(),
                    startBlocksInUse == oa.numBlocksInUse());
        } // end for (ti)
    } // end for (op)
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test28_sort()
    // Dispatcher function
{
    test28_sort(HasOperatorLessThan());
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test28_sort(bsl::false_type hasOperatorLessThan)
{
    (void) hasOperatorLessThan;

    if (verbose) printf("testSort<%s>: no intrinsic 'operator<'\n",
                                                  bsls::NameOf<TYPE>().name());

    test28_sortComp();
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test28_sort(bsl::true_type hasOperatorLessThan)
{
    (void) hasOperatorLessThan;

    if (verbose) printf("test28_sort<%s>: intrinsic 'operator<'\n",
                                                  bsls::NameOf<TYPE>().name());

    test28_sortNoComp();
    test28_sortComp();
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test28_sortComp()
{
    // ------------------------------------------------------------------------
    // TESTING SORT
    //
    // Concerns:
    //: 1 Sorts correctly in the presence of equivalent (duplicate) elements.
    //:
    //: 2 Sorts correctly if the input is already sorted or sorted in reverse.
    //:
    //: 3 No memory is allocated or deallocated during the sort.
    //:
    //: 4 No constructors, destructors, or assignment of elements takes place.
    //:
    //: 5 Iterators to all elements remain valid.
    //:
    //: 6 The predicate version of 'sort' can be used to sort using a different
    //:   comparison criterion.
    //:
    //: 7 The non-predicate version of 'sort' does not use 'std::less'.
    //:
    //: 8 The sort is stable -- equivalent elements remain in the same order as
    //:   in the original list.
    //:
    //: 9 The number of calls to the comparison operation is no larger than
    //:   'N*log2(N)', where 'N' is the number of elements.
    //:
    //: 10 If the comparison function throws an exception, no memory is
    //:    leaked.  (The order of the elements is indeterminate.)
    //
    // Test plan:
    //: 1 Create a series of list specifications of different lengths, some
    //:   containing duplicates, triplicates, and multiple sets of duplicates
    //:   and triplicates.
    //:
    //: 2 Generate every permutation of elements within each specification.
    //:
    //: 3 Create a list from the permutation, store an iterator to each list
    //:   element, and sort the list.
    //:
    //: 4 Verify that:
    //:   o The resultant list is a sorted version of the original.
    //:   o Iterating over each element in the sorted list results in an
    //:     iterator that existed in the original list.
    //:   o For equivalent elements, the iterators appear in the same order.
    //:
    //: 5 Test allocations, constructor counts, destructor counts, and
    //:   assignment counts before and after the sort and verify that they
    //:   haven't changed.  (Constructor, destructor, and assignment counts are
    //:   meaningful only if 'TYPE' is 'TestType', but will are accessible and
    //:   will remain unchanged anyway for other types.)
    //:
    //: 6 To address concern 7, std::less<TestType> is specialized to detect
    //:   being called inappropriately.
    //:
    //: 7 To address concern 6, repeat the test using a predicate that sorts in
    //:   reverse order.
    //:
    //: 8 To address concern 9, the predicate counts the number of invocations.
    //:
    //: 9 To address concern 10, the predicate operator is instrumented to
    //:   throw an exception after a specific number of iterations.
    //:
    //: 10 Using a sample string, set the comparison operator to throw at
    //:    different counts and verify, after each exception, that:
    //:    o No memory is leaked.
    //:    o The list is valid.
    //:    o Every element in the list is represented by a saved iterator.
    //
    // Testing:
    //   template <class COMP> void sort(COMP c);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    const int MAX_SPEC_LEN = 10;

    // NOTE: The elements within each of these specifications must be sorted so
    // that 'next_permutation' can do the right thing.  Since we will be
    // testing every permutation, there is no worry about having the elements
    // int too predictable an order.

    const char *const SPECS[] = {
        // Length 0 or 1: 1 permutation each
        "",
        "A",
        // Length 2: max 2 permutations each
        "AA", "AB",
        // Length 3: max 3! = 6 permutations each
        "AAA", "AAB", "ABB", "ABC",
        // Length 4: max 4! = 24 permutations each
        "AAAA", "AAAB", "AABB", "ABBB", "AABC", "ABBC", "ABCC", "ABCD",
        // Length 5: max 5! = 120 permutations each
        "AAAAA", "AAAAB", "AAABB", "AABBB", "ABBBB",
        "AAABC", "AABBC", "AABCC", "ABBBC", "ABBCC", "ABCCC", "ABCDE",
        // Length 8: max 8! = 40320 permutations each
        "ABCDEFGH", "AABCDEFG", "ABCDEFGG", "AABCDEFF", "ABCDDEFG",
        "AABCCDEE", "AAABBCDE",
        // Length 10, with no more than 8 unique elements:
        // 'max 10!/2!2! == 907200 permutations': "AABCDEFFGH"
    };

    enum { NUM_SPECS = sizeof(SPECS) / sizeof(SPECS[0]) };

    // Log2 of integers from 0 to 16, rounded up.  (Log2(0) is undefined, but 0
    // works for our purposes.)

    const int LOG2[] = { 0, 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 };

    if (verbose) printf("Testing template<COMP> sort(COMP)\n");

    for (int i = 0; i < NUM_SPECS; ++i) {
        const int LENGTH = static_cast<int>(std::strlen(SPECS[i]));
        ASSERT(MAX_SPEC_LEN >= LENGTH);

        // Copy SPECS[i] in reverse order
        char s_spec[MAX_SPEC_LEN + 1];
        for (int j = 0; j < LENGTH; ++j) {
            s_spec[j] = SPECS[i][LENGTH - j - 1];
        }
        s_spec[LENGTH] = '\0';
        const char* const S_SPEC = s_spec;    // (reverse) sorted spec.

        char spec[MAX_SPEC_LEN + 1];
        std::strcpy(spec, S_SPEC);

        // Expected result
        Obj mExp;  const Obj& EXP = gg(&mExp, S_SPEC);

        // Test each permutation
        do {
            const char *const SPEC = spec;

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            const_iterator save_iters[MAX_SPEC_LEN + 1];
            const_iterator xi = X.begin();

            for (int j = 0; j < LENGTH; ++j, ++xi) {
                save_iters[j] = xi;
            }

            save_iters[LENGTH] = xi;

            const Int64 BB = oa.numBlocksTotal();
            const Int64 B  = oa.numBlocksInUse();

            const int CTORS_BEFORE = (numDefaultCtorCalls +
                                      numIntCtorCalls     +
                                      numCopyCtorCalls);
            const int ASSIGN_BEFORE = numAssignmentCalls;
            const int DTORS_BEFORE  = numDestructorCalls;

            int predicateCalls = 0;  // Count of calls to predicate

            if (veryVeryVeryVerbose) { printf("\tBefore: "); P_(X); }

            mX.sort(GreaterThan(&predicateCalls));

            if (veryVeryVeryVerbose) { printf("After: "); P(X); }

            const Int64 AA = oa.numBlocksTotal();
            const Int64 A  = oa.numBlocksInUse();

            const int CTORS_AFTER = (numDefaultCtorCalls +
                                     numIntCtorCalls     +
                                     numCopyCtorCalls);
            const int ASSIGN_AFTER = numAssignmentCalls;
            const int DTORS_AFTER  = numDestructorCalls;

            ASSERTV(SPEC, checkIntegrity(X, LENGTH));
            ASSERTV(SPEC, X            == EXP);
            ASSERTV(SPEC, AA           == BB);
            ASSERTV(SPEC, A            == B);
            ASSERTV(SPEC, CTORS_AFTER  == CTORS_BEFORE);
            ASSERTV(SPEC, DTORS_AFTER  == DTORS_BEFORE);
            ASSERTV(SPEC, ASSIGN_AFTER == ASSIGN_BEFORE);

            // Verify complexity requirement.
            ASSERTV(SPEC, predicateCalls <= LENGTH * LOG2[LENGTH]);
            ASSERTV(SPEC, predicateCalls >= LENGTH - 1);

            xi = X.begin();
            for (int j = 0; j < LENGTH; ++j) {
                // Find index of iterator in saved iterator array
                const_iterator* p = std::find(save_iters,
                                              save_iters + LENGTH,
                                              xi);
                ptrdiff_t       save_idx = p - save_iters;

                ASSERTV(SPEC, j, LENGTH >= save_idx);

                // Verify stable sort.  Iterate through equivalent values and
                // verify that the sorted list produces iterators in the same
                // order as in the saved iterator array.  As each iterator is
                // matched, it is removed from 'save_iters' so as to ensure
                // that no iterator appears more than once (which would
                // represent a serious data structure corruption).
                char val = SPEC[save_idx];
                for (long k = save_idx; SPEC[k] == val; ++k, ++xi, ++j) {
                    ASSERTV(SPEC, k, xi == save_iters[k]);
                    save_iters[k] = X.end();  // Avoid matching iterator twice
                } // end for k
            } // end for j
        } while (std::next_permutation(spec,
                                       spec + LENGTH,
                                       std::greater<char>()));
    } // end for i

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose)
        printf("Testing exception safety of template<COMP> sort(COMP)\n");

    // Choose a longish string of random values
    const char EH_SPEC[] = "CBHADBAGCBFFADHE";
    enum {
        EH_SPEC_LEN = sizeof(EH_SPEC) - 1
    };

    bool caught_ex = true;
    for (int threshold = 0; caught_ex; ++threshold) {
        caught_ex = false;

        Obj mX(xoa);  const Obj& X = gg(&mX, EH_SPEC);

        const_iterator save_iters[EH_SPEC_LEN + 1];
        const_iterator xi = X.begin();
        for (int j = 0; j < EH_SPEC_LEN; ++j, ++xi) {
            save_iters[j] = xi;
        }
        save_iters[EH_SPEC_LEN] = xi;

        const Int64 BB = oa.numBlocksTotal();
        const Int64 B  = oa.numBlocksInUse();

        const int CTORS_BEFORE = (numDefaultCtorCalls +
                                  numIntCtorCalls     +
                                  numCopyCtorCalls);
        const int ASSIGN_BEFORE = numAssignmentCalls;
        const int DTORS_BEFORE  = numDestructorCalls;

        GreaterThan gt;  // Create a predicate object
        int         limit = threshold;
        gt.setInvocationLimit(&limit);
        try {
            mX.sort(gt);
        }
        catch (int e) {
            ASSERTV(threshold, -1 == e);
            caught_ex = true;
        }
        catch (...) {
            ASSERTV(threshold, !"Caught unexpected exception");
            caught_ex = true;
        }

        if (veryVeryVeryVerbose) {
            T_; P_(threshold); P_(caught_ex);
            printf("Result: "); P(X);
        }

        const Int64 AA = oa.numBlocksTotal();
        const Int64 A  = oa.numBlocksInUse();

        const int CTORS_AFTER = (numDefaultCtorCalls +
                                 numIntCtorCalls     +
                                 numCopyCtorCalls);
        const int ASSIGN_AFTER = numAssignmentCalls;
        const int DTORS_AFTER  = numDestructorCalls;

        if (caught_ex) {
            // Should not call predicate more than N*Log2(N) times.
            ASSERTV(threshold, threshold < EH_SPEC_LEN * LOG2[EH_SPEC_LEN]);
        }
        else {
            // Must have called predicate successfully at least N-1 times.
            ASSERTV(threshold, threshold >= EH_SPEC_LEN -1);
        }

        ASSERTV(threshold, (int) X.size() == EH_SPEC_LEN);
        ASSERTV(threshold, checkIntegrity(X, X.size()));

        ASSERTV(threshold, AA           == BB);
        ASSERTV(threshold, CTORS_AFTER  == CTORS_BEFORE);
        ASSERTV(threshold, ASSIGN_AFTER == ASSIGN_BEFORE);

        if (X.size() == EH_SPEC_LEN) {
            // To avoid cascade errors The following tests are skipped if the
            // length changed.  Otherwise they would all fail, giving no
            // additional information.

            ASSERTV(threshold, A           == B);
            ASSERTV(threshold, DTORS_AFTER == DTORS_BEFORE);
        }

        // Verify that all iterators in list were already in the list before
        // the sort (and before the exception).  The order of elements is
        // unspecified in the case of an exception, and is thus not tested.
        for (xi = X.begin(); xi != X.end(); ++xi) {
            // Find index of iterator in saved iterator array
            const_iterator* p = std::find(save_iters,
                                          save_iters + EH_SPEC_LEN,
                                          xi);
            ptrdiff_t       save_idx = p - save_iters;
            const char      VAL = EH_SPEC[save_idx];

            ASSERTV(threshold, save_idx < EH_SPEC_LEN);
            if (save_idx < EH_SPEC_LEN) {
                ASSERTV(threshold, value_of(*xi) == VAL);
            }
        } // End for (xi)
    } // End for (threshold)
#endif // BDE_BUILD_TARGET_EXC
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test28_sortNoComp()
{
    // ------------------------------------------------------------------------
    // TESTING SORT
    //
    // Concerns:
    //: 1 Sorts correctly in the presence of equivalent (duplicate) elements.
    //:
    //: 2 Sorts correctly if the input is already sorted or sorted in reverse.
    //:
    //: 3 No memory is allocated or deallocated during the sort.
    //:
    //: 4 No constructors, destructors, or assignment of elements takes place.
    //:
    //: 5 Iterators to all elements remain valid.
    //:
    //: 6 The predicate version of 'sort' can be used to sort using a different
    //:   comparison criterion.
    //:
    //: 7 The non-predicate version of 'sort' does not use 'std::less'.
    //:
    //: 8 The sort is stable -- equivalent elements remain in the same order as
    //:   in the original list.
    //:
    //: 9 The number of calls to the comparison operation is no larger than
    //:   'N*log2(N)', where 'N' is the number of elements.
    //:
    //: 10 If the comparison function throws an exception, no memory is
    //:    leaked.  (The order of the elements is indeterminate.)
    //
    // Test plan:
    //: 1 Create a series of list specifications of different lengths, some
    //:   containing duplicates, triplicates, and multiple sets of duplicates
    //:   and triplicates.
    //:
    //: 2 Generate every permutation of elements within each specification.
    //:
    //: 3 Create a list from the permutation, store an iterator to each list
    //:   element, and sort the list.
    //:
    //: 4 Verify that:
    //:   o The resultant list is a sorted version of the original.
    //:   o Iterating over each element in the sorted list results in an
    //:     iterator that existed in the original list.
    //:   o For equivalent elements, the iterators appear in the same order.
    //:
    //: 5 Test allocations, constructor counts, destructor counts, and
    //:   assignment counts before and after the sort and verify that they
    //:   haven't changed.  (Constructor, destructor, and assignment counts are
    //:   meaningful only if 'TYPE' is 'TestType', but will are accessible and
    //:   will remain unchanged anyway for other types.)
    //:
    //: 6 To address concern 7, std::less<TestType> is specialized to detect
    //:   being called inappropriately.
    //:
    //: 7 To address concern 6, repeat the test using a predicate that sorts in
    //:   reverse order.
    //:
    //: 8 To address concern 9, the predicate counts the number of invocations.
    //:
    //: 9 To address concern 10, the predicate operator is instrumented to
    //:   throw an exception after a specific number of iterations.
    //:
    //: 10 Using a sample string, set the comparison operator to throw at
    //:    different counts and verify, after each exception, that:
    //:    o No memory is leaked.
    //:    o The list is valid.
    //:    o Every element in the list is represented by a saved iterator.
    //
    // Testing:
    //   void sort();
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    const int MAX_SPEC_LEN = 10;

    // NOTE: The elements within each of these specifications must be sorted so
    // that 'next_permutation' can do the right thing.  Since we will be
    // testing every permutation, there is no worry about having the elements
    // int too predictable an order.

    const char *const SPECS[] = {
        // Length 0 or 1: 1 permutation each
        "",
        "A",
        // Length 2: max 2 permutations each
        "AA", "AB",
        // Length 3: max 3! = 6 permutations each
        "AAA", "AAB", "ABB", "ABC",
        // Length 4: max 4! = 24 permutations each
        "AAAA", "AAAB", "AABB", "ABBB", "AABC", "ABBC", "ABCC", "ABCD",
        // Length 5: max 5! = 120 permutations each
        "AAAAA", "AAAAB", "AAABB", "AABBB", "ABBBB",
        "AAABC", "AABBC", "AABCC", "ABBBC", "ABBCC", "ABCCC", "ABCDE",
        // Length 8: max 8! = 40320 permutations each
        "ABCDEFGH", "AABCDEFG", "ABCDEFGG", "AABCDEFF", "ABCDDEFG",
        "AABCCDEE", "AAABBCDE",
        // Length 10, with no more than 8 unique elements:
        // 'max 10!/2!2! == 907200 permutations'
//        "AABCDEFFGH"
    };

    enum { NUM_SPECS = sizeof(SPECS) / sizeof(SPECS[0]) };

    if (verbose) printf("Testing sort()\n");

    for (int i = 0; i < NUM_SPECS; ++i) {
        const char* const S_SPEC = SPECS[i];  // Sorted spec.
        const int         LENGTH = static_cast<int>(std::strlen(S_SPEC));
        ASSERT(MAX_SPEC_LEN >= LENGTH);

        char spec[MAX_SPEC_LEN + 1];
        std::strcpy(spec, S_SPEC);

        // Expected result
        Obj mExp;   const Obj& EXP = gg(&mExp, S_SPEC);

        // Test each permutation
        do {
            const char *const SPEC = spec;
            if (veryVeryVerbose) P(SPEC);

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            const_iterator save_iters[MAX_SPEC_LEN + 1];
            const_iterator xi = X.begin();
            for (int j = 0; j < LENGTH; ++j, ++xi) {
                save_iters[j] = xi;
            }
            save_iters[LENGTH] = xi;

            const Int64 BB = oa.numBlocksTotal();
            const Int64 B  = oa.numBlocksInUse();
            const int   CTORS_BEFORE = (numDefaultCtorCalls +
                                        numIntCtorCalls     +
                                        numCopyCtorCalls);
            const int   ASSIGN_BEFORE = numAssignmentCalls;
            const int   DTORS_BEFORE  = numDestructorCalls;

            mX.sort();

            const Int64 AA = oa.numBlocksTotal();
            const Int64 A  = oa.numBlocksInUse();
            const int   CTORS_AFTER = (numDefaultCtorCalls +
                                       numIntCtorCalls     +
                                       numCopyCtorCalls);
            const int   ASSIGN_AFTER = numAssignmentCalls;
            const int   DTORS_AFTER  = numDestructorCalls;

            ASSERTV(SPEC, checkIntegrity(X, LENGTH));
            ASSERTV(SPEC, X            == EXP);
            ASSERTV(SPEC, AA           == BB);
            ASSERTV(SPEC, A            == B);
            ASSERTV(SPEC, CTORS_AFTER  == CTORS_BEFORE);
            ASSERTV(SPEC, DTORS_AFTER  == DTORS_BEFORE);
            ASSERTV(SPEC, ASSIGN_AFTER == ASSIGN_BEFORE);

            xi = X.begin();
            for (int j = 0; j < LENGTH; ++j) {
                // Find index of iterator in saved iterator array
                const_iterator* p = std::find(save_iters,
                                              save_iters + LENGTH,
                                              xi);
                long            save_idx = p - save_iters;

                ASSERTV(SPEC, j, LENGTH >= save_idx);

                // Verify stable sort.  Iterate through equivalent values and
                // verify that the sorted list produces iterators in the same
                // order as in the saved iterator array.  As each iterator is
                // matched, it is removed from 'save_iters' so as to ensure
                // that no iterator appears more than once (which would
                // represent a serious data structure corruption).
                char val = SPEC[save_idx];
                for (long k = save_idx; SPEC[k] == val; ++k, ++xi, ++j) {
                    ASSERTV(SPEC, k, xi == save_iters[k]);
                    save_iters[k] = X.end();  // Avoid matching iterator twice
                } // end for k
            } // end for j
        } while (std::next_permutation(spec, spec + LENGTH));
    } // end for i
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test27_merge()
{
    test27_merge(HasOperatorLessThan());
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test27_merge(bsl::false_type)
{
    test27_mergeComp();
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test27_merge(bsl::true_type)
{
    test27_mergeNoComp();
    test27_mergeComp();
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test27_mergeComp()
{
    // ------------------------------------------------------------------------
    // TESTING MERGE
    //
    // Concerns:
    //: 1 Merging produces correct results with and without duplicate elements
    //:   within and between the lists to be merged.
    //:
    //: 2 The argument to merge is empty after the merge.
    //:
    //: 3 No memory is allocated or deallocated during the merge.
    //:
    //: 4 No constructors, destructors, or assignment of elements takes place.
    //:
    //: 5 Iterators to all elements remain valid.
    //:
    //: 6 The predicate version of 'merge' can be used to merge using a
    //:   different comparison criterion.
    //:
    //: 7 The non-predicate version of 'merge' does not use 'std::less'.
    //:
    //: 8 Merging a list with itself has no effect.
    //:
    //: 9 If the comparison function throws an exception, no memory is leaked
    //:   and all elements remain in one list or the other.
    //
    // Test plan:
    //: 1 Create two lists from the cross-product of two small sets of
    //:   specifications.  The elements in the lists are chosen so that every
    //:   combination of duplicate and non-duplicate elements, both within and
    //:   between lists, is represented.
    //:
    //: 2 Save the iterators to all elements of both lists and record the
    //:   memory usage before the merge.
    //:
    //: 3 Merge one list into the other.
    //:
    //: 4 Verify that:
    //:   o The merged value is correct.
    //:   o All of the pre-merge iterators are still valid.
    //:   o The non-merged list is now empty.
    //:
    //: 5 To address concern 6, sort the initial specifications using the
    //:   reverse sort order, then use a custom "greater-than" predicate to
    //:   merge the lists and verify the same values as for the non-predicate
    //:   case.
    //:
    //: 6 To address concern 7, std::less<TestType> is specialized to detect
    //:   being called inappropriately.
    //:
    //: 7 To address concern 8, merge each list with itself and verify that no
    //:   memory is allocated or deallocated and that all iterators remain
    //:   valid.
    //
    // Testing:
    //   template <class COMP> void merge(list& other, COMP c);
    //   template <class COMP> void merge(list&& other, COMP c);
    // ------------------------------------------------------------------------

    TestValues VALUES("ABCDEFGH");

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    const ALLOC          xoa(&oa);

    const int MAX_SPEC_LEN = SortedSpecGen::MAX_SPEC_LEN;

    for (int doMove = 0; doMove < 2; ++doMove) {
        if (verbose) {
            printf(doMove ? "Testing void merge(list&& other, COMP c);\n"
                          : "Testing void merge(list& other, COMP c);\n");
        }
        for (SortedSpecGen xgen; xgen; ++xgen) {
            for (SortedSpecGen ygen; ygen; ++ygen) {
                const char* const X_SPEC     = xgen.reverse_spec();
                const int         X_SPEC_LEN = xgen.len();
                const char* const Y_SPEC     = ygen.reverse_spec();
                const int         Y_SPEC_LEN = ygen.len();

                Obj mX(xoa);  const Obj& X = gg(&mX, X_SPEC);
                Obj mY(xoa);  const Obj& Y = gg(&mY, Y_SPEC);

                const_iterator xiters[MAX_SPEC_LEN + 1];
                const_iterator yiters[MAX_SPEC_LEN + 1];

                // Save the iterators before merge
                int xi = 0;
                for (const_iterator it = X.begin(); it != X.end(); ++it, ++xi){
                    xiters[xi] = it;
                }
                xiters[xi] = X.end();
                int yi = 0;
                for (const_iterator it = Y.begin(); it != Y.end(); ++it, ++yi){
                    yiters[yi] = it;
                }
                yiters[yi] = Y.end();

                const Int64 BB = oa.numBlocksTotal();
                const Int64 B  = oa.numBlocksInUse();

                const int CTORS_BEFORE = (numDefaultCtorCalls +
                                          numIntCtorCalls     +
                                          numCopyCtorCalls);
                const int ASSIGN_BEFORE = numAssignmentCalls;
                const int DTORS_BEFORE  = numDestructorCalls;

                // Self merge (noop)

                if (doMove) {
                    mX.merge(MoveUtil::move(mX), GreaterThan());
                }
                else {
                    mX.merge(mX, GreaterThan());
                }

                ASSERTV(X_SPEC, Y_SPEC, (int) X.size() == X_SPEC_LEN);
                ASSERTV(X_SPEC, Y_SPEC, (int) Y.size() == Y_SPEC_LEN);

                if (veryVeryVerbose) {
                    T_; printf("Before: "); P_(X); P_(Y);
                }

                // Test merge here

                if (doMove) {
                    mX.merge(MoveUtil::move(mY), GreaterThan());
                }
                else {
                    mX.merge(mY, GreaterThan());
                }

                if (veryVeryVerbose) {
                    T_; printf("After: "); P_(X); P(Y);
                }

                const Int64 AA = oa.numBlocksTotal();
                const Int64 A  = oa.numBlocksInUse();

                const int CTORS_AFTER = (numDefaultCtorCalls +
                                         numIntCtorCalls     +
                                         numCopyCtorCalls);
                const int ASSIGN_AFTER = numAssignmentCalls;
                const int DTORS_AFTER  = numDestructorCalls;

                // Test result size
                ASSERTV(X_SPEC, Y_SPEC,
                        (int) X.size() == X_SPEC_LEN + Y_SPEC_LEN);
                ASSERTV(X_SPEC, Y_SPEC, Y.size() == 0);

                // Test merged results and iterators
                int idx = 0;
                xi = yi = 0;
                for (const_iterator it = X.begin(); it != X.end(); ++it, ++idx)
                {
                    if (it == xiters[xi]) {
                        if (yi < Y_SPEC_LEN) {
                            // Verify that merge criterion was met
                            ASSERTV(X_SPEC, Y_SPEC, xi, yi,
                                    !(Y_SPEC[yi] > X_SPEC[xi]));
                            ASSERTV(X_SPEC, Y_SPEC, xi, yi,
                                    VALUES[X_SPEC[xi] - 'A'] == *it);
                        }
                        ++xi;
                    }
                    else if (it == yiters[yi]) {
                        if (xi < X_SPEC_LEN) {
                            // Verify that merge criterion was met.
                            // C++98 required that items from X precede
                            // equivalent items from Y.  C++0x seemed to remove
                            // this requirement, but we should adhere to it
                            // anyway.

                            ASSERTV(X_SPEC, Y_SPEC, xi, yi,
                                    Y_SPEC[yi] > X_SPEC[xi]);
                            ASSERTV(X_SPEC, Y_SPEC, xi, yi,
                                    VALUES[Y_SPEC[yi] - 'A'] == *it);
                        }
                        ++yi;
                    }
                    else {
                        // A stable merge requires that the iterator must match
                        // the next iterator on the save x or y list.

                        ASSERTV(X_SPEC, Y_SPEC, xi, yi, !"Invalid merge");
                    }
                }
                // Test end iterators

                ASSERTV(X_SPEC, Y_SPEC, xi, yi, xiters[xi] == X.end());
                ASSERTV(X_SPEC, Y_SPEC, xi, yi, yiters[yi] == Y.end());
                ASSERTV(X_SPEC, Y_SPEC, xi, yi, (xi + yi) == (int) X.size());

                // Test allocations and deallocations

                ASSERTV(X_SPEC, Y_SPEC, xi, yi, AA == BB);
                ASSERTV(X_SPEC, Y_SPEC, xi, yi,  A ==  B);

                // Test that no constructors, destructors or assignments were
                // called.

                ASSERTV(X_SPEC, Y_SPEC, xi, yi, CTORS_AFTER == CTORS_BEFORE);
                ASSERTV(X_SPEC, Y_SPEC, xi, yi, DTORS_AFTER == DTORS_BEFORE);
                ASSERTV(X_SPEC, Y_SPEC, xi, yi, ASSIGN_AFTER == ASSIGN_BEFORE);
            } // end for (ygen)
        } // end for (xgen)
    } // end for (doMove)

#ifdef BDE_BUILD_TARGET_EXC
    for (int doMove = 0; doMove < 2; ++doMove) {
        if (verbose) printf(doMove ? "Testing exception safety with move\n"
                                   : "Testing exception safety\n");

        const char X_SPEC[] = "HGFEDCBA";
        const char Y_SPEC[] = "GGEECCBA";

        enum {
            X_SPEC_LEN = sizeof(X_SPEC) - 1,
            Y_SPEC_LEN = sizeof(Y_SPEC) - 1,
            MERGED_SPEC_LEN = X_SPEC_LEN + Y_SPEC_LEN
        };

        bool caught_ex = true;
        for (int threshold = 0; caught_ex; ++threshold) {
            caught_ex = false;

            Obj mX(xoa);  const Obj& X = gg(&mX, X_SPEC);
            Obj mY(xoa);  const Obj& Y = gg(&mY, Y_SPEC);

            const_iterator save_iters[MERGED_SPEC_LEN + 1];
            int            j = 0;

            for (const_iterator xi = X.begin(); xi != X.end(); ++xi, ++j) {
                save_iters[j] = xi;
            }

            for (const_iterator yi = Y.begin(); yi != Y.end(); ++yi, ++j) {
                save_iters[j] = yi;
            }

            save_iters[MERGED_SPEC_LEN] = Y.end();

            ASSERTV(threshold, MERGED_SPEC_LEN == j);

            const Int64 BB = oa.numBlocksTotal();
            const Int64 B  = oa.numBlocksInUse();

            const int CTORS_BEFORE = (numDefaultCtorCalls +
                                      numIntCtorCalls     +
                                      numCopyCtorCalls);
            const int ASSIGN_BEFORE = numAssignmentCalls;
            const int DTORS_BEFORE  = numDestructorCalls;

            GreaterThan gt;  // Create a predicate object
            int         limit = threshold;
            gt.setInvocationLimit(&limit);
            try {
                if (doMove) {
                    mX.merge(MoveUtil::move(mY), gt);
                }
                else {
                    mX.merge(mY, gt);
                }
            }
            catch (int e) {
                ASSERTV(threshold, -1 == e);
                caught_ex = true;
            }
            catch (...) {
                ASSERTV(threshold, !"Caught unexpected exception");
                caught_ex = true;
            }

            if (veryVeryVeryVerbose) {
                T_; P_(threshold); P_(caught_ex);
                printf("Result: "); P_(X); P(Y);
            }

            const Int64 AA = oa.numBlocksTotal();
            const Int64 A  = oa.numBlocksInUse();

            const int CTORS_AFTER = (numDefaultCtorCalls +
                                     numIntCtorCalls     +
                                     numCopyCtorCalls);
            const int ASSIGN_AFTER = numAssignmentCalls;
            const int DTORS_AFTER  = numDestructorCalls;

            ASSERTV(threshold, X.size() + Y.size() == MERGED_SPEC_LEN);
            ASSERTV(threshold, checkIntegrity(X, X.size()));
            ASSERTV(threshold, checkIntegrity(Y, Y.size()));

            ASSERTV(threshold, AA           == BB);
            ASSERTV(threshold, ASSIGN_AFTER == ASSIGN_BEFORE);
            ASSERTV(threshold, CTORS_AFTER  == CTORS_BEFORE);

            if (X.size() + Y.size() == MERGED_SPEC_LEN) {
                // To avoid cascade errors The following tests are skipped if
                // the total length changed.  Otherwise they would all fail,
                // giving no additional information.
                ASSERTV(threshold, A            == B);
                ASSERTV(threshold, DTORS_AFTER  == DTORS_BEFORE);
            }

            ASSERT(caught_ex || Y.empty());

            // Verify that all iterators in the lists were already in the lists
            // before the merge (and before the exception).  The order of
            // elements is unspecified in the case of an exception, and is thus
            // not tested.

            char prev_val = 'Z';
            for (const_iterator xi = X.begin(); xi != X.end(); ++xi) {
                // Find index of iterator in saved iterator array
                const_iterator* p = std::find(save_iters,
                                              save_iters + MERGED_SPEC_LEN,
                                              xi);
                ptrdiff_t       save_idx = p - save_iters;
                const char      VAL = (save_idx < X_SPEC_LEN)
                                               ? X_SPEC[save_idx]
                                               : Y_SPEC[save_idx - X_SPEC_LEN];

                ASSERTV(threshold, save_idx < MERGED_SPEC_LEN);
                if (save_idx < MERGED_SPEC_LEN) {
                    ASSERTV(threshold, value_of(*xi) == VAL);
                    // Verify that the values are still in descending value,
                    // even though the standard does not seem to require it.
                    ASSERTV(threshold, VAL <= prev_val);
                }
                save_iters[save_idx] = Y.end(); // Prevent duplicate matches
            } // End for (xi)
            prev_val = 'Z';
            for (const_iterator yi = Y.begin(); yi != Y.end(); ++yi) {
                // Find index of iterator in saved iterator array
                const_iterator* p = std::find(save_iters,
                                              save_iters + MERGED_SPEC_LEN,
                                              yi);
                ptrdiff_t       save_idx = p - save_iters;
                const char      VAL = (save_idx < X_SPEC_LEN)
                                               ? X_SPEC[save_idx]
                                               : Y_SPEC[save_idx - X_SPEC_LEN];

                ASSERTV(threshold, save_idx < MERGED_SPEC_LEN);
                if (save_idx < MERGED_SPEC_LEN) {
                    ASSERTV(threshold, value_of(*yi) == VAL);
                    // Verify that the values are still in descending value,
                    // even though the standard does not seem to require it.
                    ASSERTV(threshold, VAL <= prev_val);
                }
                save_iters[save_idx] = Y.end(); // Prevent duplicate matches
            } // End for (yi)
        } // End for (threshold)
    }
#endif // BDE_BUILD_TARGET_EXC
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test27_mergeNoComp()
{
    // ------------------------------------------------------------------------
    // TESTING MERGE
    //
    // Concerns:
    //: 1 Merging produces correct results with and without duplicate elements
    //:   within and between the lists to be merged.
    //:
    //: 2 The argument to merge is empty after the merge.
    //:
    //: 3 No memory is allocated or deallocated during the merge.
    //:
    //: 4 No constructors, destructors, or assignment of elements takes place.
    //:
    //: 5 Iterators to all elements remain valid.
    //:
    //: 6 The predicate version of 'merge' can be used to merge using a
    //:   different comparison criterion.
    //:
    //: 7 The non-predicate version of 'merge' does not use 'std::less'.
    //:
    //: 8 Merging a list with itself has no effect.
    //:
    //: 9 If the comparison function throws an exception, no memory is leaked
    //:   and all elements remain in one list or the other.
    //
    // Test plan:
    //: 1 Create two lists from the cross-product of two small sets of
    //:   specifications.  The elements in the lists are chosen so that every
    //:   combination of duplicate and non-duplicate elements, both within and
    //:   between lists, is represented.
    //:
    //: 2 Save the iterators to all elements of both lists and record the
    //:   memory usage before the merge.
    //:
    //: 3 Merge one list into the other.
    //:
    //: 4 Verify that:
    //:   o The merged value is correct.
    //:   o All of the pre-merge iterators are still valid.
    //:   o The non-merged list is now empty.
    //:
    //: 5 To address concern 6, sort the initial specifications using the
    //:   reverse sort order, then use a custom "greater-than" predicate to
    //:   merge the lists and verify the same values as for the non-predicate
    //:   case.
    //:
    //: 6 To address concern 7, std::less<TestType> is specialized to detect
    //:   being called inappropriately.
    //:
    //: 7 To address concern 8, merge each list with itself and verify that no
    //:   memory is allocated or deallocated and that all iterators remain
    //:   valid.
    //
    // Testing:
    //   void merge(list& other);
    //   void merge(list&& other);
    // ------------------------------------------------------------------------

    TestValues VALUES("ABCDEFGH");

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    const ALLOC          xoa(&oa);

    const int MAX_SPEC_LEN = SortedSpecGen::MAX_SPEC_LEN;

    for (int doMove = 0; doMove < 2; ++doMove) {
        if (verbose) printf(doMove ? "Testing void merge(list&& other);\n"
                                   : "Testing void merge(list& other);\n");

        bool first5X = true;

        for (SortedSpecGen xgen; xgen; ++xgen) {
            const char* const X_SPEC     = xgen.spec();
            const int         X_SPEC_LEN = xgen.len();

            if (5 == X_SPEC_LEN && first5X) {
                first5X = false;
            }
            else if (3 < X_SPEC_LEN) {
                continue;
            }

            bool first5Y = true;

            for (SortedSpecGen ygen; ygen; ++ygen) {
                const char* const Y_SPEC     = ygen.spec();
                const int         Y_SPEC_LEN = ygen.len();

                if (5 == Y_SPEC_LEN && first5Y) {
                    first5Y = false;
                }
                else if (3 < Y_SPEC_LEN) {
                    continue;
                }

                Obj mX(xoa);  const Obj& X = gg(&mX, X_SPEC);
                Obj mY(xoa);  const Obj& Y = gg(&mY, Y_SPEC);

                const_iterator xiters[MAX_SPEC_LEN + 1];
                const_iterator yiters[MAX_SPEC_LEN + 1];

                // Save the iterators before merge
                int xi = 0;
                for (const_iterator it = X.begin(); it != X.end(); ++it, ++xi){
                    xiters[xi] = it;
                }
                xiters[xi] = X.end();
                int yi = 0;
                for (const_iterator it = Y.begin(); it != Y.end(); ++it, ++yi){
                    yiters[yi] = it;
                }
                yiters[yi] = Y.end();

                const Int64 BB = oa.numBlocksTotal();
                const Int64 B  = oa.numBlocksInUse();

                const int CTORS_BEFORE = (numDefaultCtorCalls +
                                          numIntCtorCalls     +
                                          numCopyCtorCalls);
                const int ASSIGN_BEFORE = numAssignmentCalls;
                const int DTORS_BEFORE  = numDestructorCalls;

                // Self merge (noop)
                if (doMove) {
                    mX.merge(MoveUtil::move(mX));
                }
                else {
                    mX.merge(mX);
                }

                ASSERTV(X_SPEC, Y_SPEC, (int) X.size() == X_SPEC_LEN);
                ASSERTV(X_SPEC, Y_SPEC, (int) Y.size() == Y_SPEC_LEN);

                if (veryVeryVerbose) {
                    T_; printf("Before: "); P_(X); P_(Y);
                }

                if (doMove) {
                    mX.merge(MoveUtil::move(mY)); // Test merge here
                }
                else {
                    mX.merge(mY); // Test merge here
                }

                if (veryVeryVerbose) {
                    T_; printf("After: "); P_(X); P(Y);
                }

                const Int64 AA = oa.numBlocksTotal();
                const Int64 A  = oa.numBlocksInUse();

                const int CTORS_AFTER = (numDefaultCtorCalls +
                                         numIntCtorCalls     +
                                         numCopyCtorCalls);
                const int ASSIGN_AFTER = numAssignmentCalls;
                const int DTORS_AFTER  = numDestructorCalls;

                // Test result size
                ASSERTV(X_SPEC, Y_SPEC,
                        (int) X.size() == X_SPEC_LEN + Y_SPEC_LEN);
                ASSERTV(X_SPEC, Y_SPEC, Y.size() == 0);

                // Test merged results and iterators
                int idx = 0;
                xi = yi = 0;
                for (const_iterator it = X.begin(); it != X.end(); ++it, ++idx)
                {
                    if (it == xiters[xi]) {
                        if (yi < Y_SPEC_LEN) {
                            // Verify that merge criterion was met
                            ASSERTV(X_SPEC, Y_SPEC, xi, yi,
                                    !(Y_SPEC[yi] < X_SPEC[xi]));
                            ASSERTV(X_SPEC, Y_SPEC, xi, yi,
                                    VALUES[X_SPEC[xi] - 'A'] == *it);
                        }
                        ++xi;
                    }
                    else if (it == yiters[yi]) {
                        if (xi < X_SPEC_LEN) {
                            // Verify that merge criterion was met.
                            // C++98 required that items from X precede
                            // equivalent items from Y.  C++0x seemed to remove
                            // this requirement, but we should adhere to it
                            // anyway.

                            ASSERTV(X_SPEC, Y_SPEC, xi, yi,
                                    Y_SPEC[yi] < X_SPEC[xi]);
                            ASSERTV(X_SPEC, Y_SPEC, xi, yi,
                                    VALUES[Y_SPEC[yi] - 'A'] == *it);
                        }
                        ++yi;
                    }
                    else {
                        // A stable merge requires that the iterator must match
                        // the next iterator on the save x or y list.
                        ASSERTV(X_SPEC, Y_SPEC, xi, yi, !"Invalid merge");
                    }
                }
                // Test end iterators
                ASSERTV(X_SPEC, Y_SPEC, xi, yi, xiters[xi] == X.end());
                ASSERTV(X_SPEC, Y_SPEC, xi, yi, yiters[yi] == Y.end());
                ASSERTV(X_SPEC, Y_SPEC, xi, yi, (xi + yi) == (int) X.size());

                // Test allocations and deallocations
                ASSERTV(X_SPEC, Y_SPEC, xi, yi, AA == BB);
                ASSERTV(X_SPEC, Y_SPEC, xi, yi,  A ==  B);

                // Test that no constructors, destructors or assignments were
                // called.
                ASSERTV(X_SPEC, Y_SPEC, xi, yi, CTORS_AFTER  == CTORS_BEFORE);
                ASSERTV(X_SPEC, Y_SPEC, xi, yi, DTORS_AFTER  == DTORS_BEFORE);
                ASSERTV(X_SPEC, Y_SPEC, xi, yi, ASSIGN_AFTER == ASSIGN_BEFORE);
            } // end for (ygen)
        } // end for (xgen)
    } // end for (doMove)
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test26_unique()
{
    // ------------------------------------------------------------------------
    // TESTING UNIQUE:
    //
    // Concerns:
    //: 1 The predicate and non-predicate versions of 'unique' have essentially
    //:   the same characteristics.
    //:
    //: 2 Can remove elements from any or all positions in the list except the
    //:   first.
    //:
    //: 3 Destructors are called for removed elements and memory is deleted for
    //:   removed elements.
    //:
    //: 4 No constructors, destructors, or assignment operators are called on
    //:   the remaining (non-removed) elements.
    //:
    //: 5 No memory is allocated.
    //:
    //: 6 Iterators to non-removed elements, including the 'end()' iterator,
    //:   remain valid after removal.
    //:
    //: 7 The non-removed elements retain their relative order.
    //:
    //: 8 The 'unique' operation is exception-neutral, if the equality operator
    //:   or predicate throw an exception.
    //:
    //: 9 The non-predicate version calls operator==(T,T) directly; it does not
    //:   call std::equal_to<T>::operator()(T,T).
    //
    // Plan:
    //: 1 For concern 1, perform the same tests for both the predicate and
    //:   non-predicate versions of 'unique.
    //:
    //: 2 Generate lists of various lengths up to 10 elements, filling the
    //:   lists with different sequences of values such that every combination
    //:   of matching and non-matching subsequences is generated.  (For the
    //:   predicate version, matching elements need to be equal only in their
    //:   low bit).
    //:
    //: 3 For each combination, make a copy of all of the iterators to
    //:   non-repeated elements, then call 'unique'.
    //:
    //: 4 Validate that:
    //:   o The number of new destructor calls matches the number of elements
    //:     removed.
    //:   o Reduction of memory blocks in use is correct for the number
    //:     elements removed.
    //:   o The number of new allocations is zero, the number of new
    //:     constructor calls is zero.
    //:   o The iterating over the remaining elements produces a sequence of
    //:     values and iterators matching those saved before the 'unique'
    //:     operation.
    //:
    //: 5 For concern 8, perform the tests in an exception-testing framework,
    //:   using a special feature of the 'LowBitEQ' predicate to cause
    //:   exceptions to be thrown.
    //:
    //: 6 For concern 9, std::equal_to<TestType> is specialized to detect being
    //:   called inappropriately.
    //
    // Testing:
    //   void unique();
    //   template <class BINPRED> void unique(BINPRED p);
    // ------------------------------------------------------------------------

    // For this test, it is important that 'NUM_VALUES' be even.
    TestValues VALUES("ABCDEFGH");
    const int  NUM_VALUES = 8;

    bslma::TestAllocator testAllocator; // For exception testing only
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    const int LENGTHS[] = { 0, 1, 2, 3, 4, 5, 10 };
    enum { NUM_LENGTHS = sizeof(LENGTHS) / sizeof(LENGTHS[0]) };
    const int MAX_LENGTH = 10;

    enum {
        OP_FIRST,
        OP_UNIQUE = OP_FIRST, // void unique();
        OP_UNIQUE_PRED,       // template <class BINPRED> void unique(BINPRED);
        OP_LAST
    };

    for (int op = OP_FIRST; op < OP_LAST; ++op) {

        // The 'perturb_bit' is a bit mask that can be perturbed in a value and
        // still compare equal to the original according to the predicate.
        char perturb_bit;

        switch (op) {
          case OP_UNIQUE:
            if (verbose) printf("Testing unique()\n");
            perturb_bit = 0;
            break;
          case OP_UNIQUE_PRED:
            if (verbose) printf("Testing unique(BINPRED p)\n");
            perturb_bit = 2;
            break;
          default:
            ASSERT(!"Bad operation requested for test");
            return;                                                   // RETURN
        }

        for (int i = 0; i < NUM_LENGTHS; ++i) {
            const unsigned LEN  = LENGTHS[i];

            ASSERT(MAX_LENGTH >= LEN);

            // 'mask' contains a bit for each element in the list.  For each
            // '1' bit, the element should match the preceding element's value
            // according to the predicate.  Bit 0 (the first position) is
            // skipped, since it has no preceding value.

            for (unsigned mask = 0; mask < (unsigned(1) << LEN); mask += 2) {

              BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                const_iterator save_iters[MAX_LENGTH + 1];
                int            res_len = 0;  // To compute exp. result length

                Obj mX(xoa);  const Obj& X = mX;            // test objected
                Obj res_exp;  const Obj& RES_EXP = res_exp; // expected result

                int val_idx  = 0;
                if (LEN > 0) {
                    primaryCopier(&mX, VALUES[0]);
                    primaryCopier(&res_exp, VALUES[0]);
                    ++res_len;
                    save_iters[0] = X.begin();
                }

                for (unsigned bit = 2; bit < (unsigned(1) << LEN); bit <<= 1) {
                    if ((mask & bit)) {
                        // Set the new value to the previous value, but
                        // (possibly) perturbed in such a way that it they
                        // still compare equal according to the predicate.
                        val_idx = val_idx ^ perturb_bit;
                        primaryCopier(&mX, VALUES[val_idx]);
                    }
                    else {
                        // Increment val_idx, modulo NUM_VALUES
                        val_idx = (val_idx + 1) % NUM_VALUES;
                        primaryCopier(&mX, VALUES[val_idx]);
                        primaryCopier(&res_exp, VALUES[val_idx]);

                        // Save iterators to non-repeated elements
                        save_iters[res_len++] = --X.end();
                    }
                }

                ASSERTV(op, X, RES_EXP, X.size() == LEN);
                ASSERTV(op, X, RES_EXP, (int) RES_EXP.size() == res_len);

                save_iters[res_len] = X.end();

                const Int64 BB = oa.numBlocksTotal();
                const Int64 B  = oa.numBlocksInUse();

                const int CTORS_BEFORE = (numDefaultCtorCalls +
                                          numIntCtorCalls     +
                                          numCopyCtorCalls);
                const int ASSIGN_BEFORE = numAssignmentCalls;
                const int DTORS_BEFORE  = numDestructorCalls;

                if (veryVeryVerbose) { T_; printf("Before: "); P_(X); }

                switch (op) {
                  case OP_UNIQUE:
                    mX.unique();
                    break;
                  case OP_UNIQUE_PRED: {
                    mX.unique(LowBitEQ(&testAllocator));
                  } break;
                }

                if (veryVeryVerbose) { printf("After: "); P(X); }

                const Int64 AA = oa.numBlocksTotal();
                const Int64 A  = oa.numBlocksInUse();

                const int CTORS_AFTER = (numDefaultCtorCalls +
                                         numIntCtorCalls     +
                                         numCopyCtorCalls);
                const int ASSIGN_AFTER = numAssignmentCalls;
                const int DTORS_AFTER  = numDestructorCalls;

                // Test result value
                ASSERTV(op, X, RES_EXP, checkIntegrity(X, res_len));
                ASSERTV(op, X, RES_EXP, (int) X.size() == res_len);
                ASSERTV(op, X, RES_EXP, X == RES_EXP);

                // Test that iterators are still valid
                int idx = 0;
                for (const_iterator it = X.begin(); it != X.end(); ++it, ++idx)
                {
                    ASSERTV(op, X, RES_EXP, idx, save_iters[idx] == it);
                }

                // Test end iterator
                ASSERTV(op, X, RES_EXP, idx, save_iters[idx] == X.end());

                // Test allocations and deallocations
                ASSERTV(op, X, RES_EXP, AA == BB);
                ASSERTV(op, X, RES_EXP, deltaBlocks(LEN - res_len) == B - A);

                // If 'TYPE' is 'TestType', then test that no constructors or
                // assignments were called and the expected number of
                // destructors were called.
                if (bsl::is_same<TYPE, TestType>::value) {
                    ASSERTV(op, X, RES_EXP, CTORS_AFTER  == CTORS_BEFORE);
                    ASSERTV(op, X, RES_EXP, ASSIGN_AFTER == ASSIGN_BEFORE);
                    ASSERTV(op, X, RES_EXP,
                                         CMPINT(DTORS_AFTER,
                                                ==,
                                                DTORS_BEFORE + (LEN-res_len)));
                }

              } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            } // end for (mask)
        } // end for (i)
    } // end for (op)
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test25_remove()
{
    // ------------------------------------------------------------------------
    // TESTING REMOVE
    //
    // Concerns:
    //: 1 'remove' and 'remove_if' have essentially the same characteristics.
    //:
    //: 2 Will remove 0..N elements from an N-element list.
    //:
    //: 3 Can remove elements from any or all positions in the list
    //:
    //: 4 Destructors are called for removed elements and memory is deleted for
    //:   removed elements.
    //:
    //: 5 No constructors, destructors, or assignment operators are called on
    //:   the remaining (non-removed) elements.
    //:
    //: 6 No memory is allocated.
    //:
    //: 7 Iterators to non-removed elements, including the 'end()' iterator,
    //:   remain valid after removal.
    //:
    //: 8 The non-'E' elements retain their relative order.
    //
    // Plan:
    //: 1 For concern 1, perform the same tests for both 'remove' and
    //:   'remove_if'.
    //:
    //: 2 Generate lists from a small set of specifications from empty to 10
    //:   elements, none of which contain the value 'E'.
    //:
    //: 3 Replace 0 to 'LENGTH' elements with the value 'E', in every possible
    //:   combination.
    //:
    //: 4 For each specification and combination, make a copy of all of the
    //:   iterators to non-'E' elements, then call 'remove' or 'remove_if'.
    //:
    //: 5 Validate that:
    //:   o The number of new destructor call matches the number of elements
    //:     removed.
    //:   o Reduction of memory blocks in use is correct for the number
    //:     elements removed.
    //:   o The number of new allocations is zero.
    //:   o The number of new constructor calls is zero.
    //:   o The iterating over the remaining elements produces a sequence of
    //:     values and iterators matching those saved before the remove
    //:     operation.
    //
    // Testing:
    //   void remove(const T& val);
    //   template <class PRED> void remove_if(PRED p);
    // ------------------------------------------------------------------------

    TestValues   VALUES("ABCDEFGH");
    const TYPE&  E = VALUES[4];  // Element with value 'E'

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    // Specifications from 0 to 10 elements long, none of which have the value
    // 'E'.
    const char* const SPECS[] = {
        "", "A", "AB", "ABA", "ABCD", "AAAA", "ABCDF", "ABCDFGHDAB"
    };

    enum { NUM_SPECS = sizeof(SPECS) / sizeof(SPECS[0]) };
    const int MAX_SPEC_LEN = 10;

    enum {
        OP_FIRST,
        OP_REMOVE = OP_FIRST, // remove(const T& val);
        OP_REMOVE_IF,         // template <class PRED> void remove_if(PRED p);
        OP_LAST
    };

    for (int op = OP_FIRST; op < OP_LAST; ++op) {

        if (verbose) {
            switch (op) {
              case OP_REMOVE:
                printf("Testing remove(const T& val)\n");
                break;
              case OP_REMOVE_IF:
                printf("Testing remove(PRED p)\n");
                break;
            }
        }

        for (int i = 0; i < NUM_SPECS; ++i) {
            const char* const SPEC = SPECS[i];
            const int         LEN  = static_cast<int>(std::strlen(SPEC));

            ASSERT(MAX_SPEC_LEN >= LEN);

            // 'mask' contains a bit for each element in the list.  For each
            // '1' bit, the element is replaced by the value 'E'

            for (unsigned mask = 0; mask < (unsigned(1) << LEN); ++mask) {

                Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                const_iterator save_iters[MAX_SPEC_LEN + 1];

                char res_spec[MAX_SPEC_LEN + 1]; // expected result spec
                int  res_len = 0;  // To compute expected result length

                // Replace each element in 'mX' for which 'mask' has a '1' bit
                // with the value 'E'.

                iterator it = mX.begin();
                unsigned idx = 0;
                for (unsigned bit = 1; bit < (unsigned(1) << LEN);
                                                      bit <<= 1, ++it, ++idx) {
                    if ((mask & bit)) {
                        assignTo(bsls::Util::addressOf(*it), value_of(E));
                    }
                    else {
                        save_iters[res_len] = it;
                        res_spec[res_len] = SPEC[idx];
                        ++res_len;
                    }
                }
                ASSERTV(SPEC, mask, X.end() == it);
                save_iters[res_len] = X.end();
                res_spec[res_len] = '\0';

                Obj mRes;  const Obj& RES = gg(&mRes, res_spec);

                const Int64 BB = oa.numBlocksTotal();
                const Int64 B  = oa.numBlocksInUse();

                const int CTORS_BEFORE = (numDefaultCtorCalls +
                                          numIntCtorCalls     +
                                          numCopyCtorCalls);
                const int ASSIGN_BEFORE = numAssignmentCalls;
                const int DTORS_BEFORE  = numDestructorCalls;

                if (veryVeryVerbose) { T_; printf("Before: "); P_(X); }

                switch (op) {
                  case OP_REMOVE:    mX.remove(E);           break;
                  case OP_REMOVE_IF: mX.remove_if(VPred(E)); break;
                }

                if (veryVeryVerbose) { printf("After: "); P(X); }

                const Int64 AA = oa.numBlocksTotal();
                const Int64 A  = oa.numBlocksInUse();

                const int CTORS_AFTER = (numDefaultCtorCalls +
                                         numIntCtorCalls     +
                                         numCopyCtorCalls);
                const int ASSIGN_AFTER = numAssignmentCalls;
                const int DTORS_AFTER  = numDestructorCalls;

                // Test result value
                ASSERTV(SPEC, res_spec, X, checkIntegrity(X, res_len));
                ASSERTV(SPEC, res_spec, X, (int) X.size() == res_len);
                ASSERTV(SPEC, res_spec, X, RES == X);

                // Test that iterators are still valid
                const_iterator cit = X.begin();
                for (idx = 0; idx < X.size(); ++idx, ++cit) {
                    ASSERTV(SPEC, res_spec, idx, save_iters[idx] == cit);
                }
                // Test end iterator
                ASSERTV(SPEC, res_spec, idx, save_iters[idx] == cit);

                // Test allocations and deallocations
                ASSERTV(SPEC, res_spec, AA == BB);
                ASSERTV(SPEC, res_spec, deltaBlocks(LEN - res_len) == B - A);

                // If 'TYPE' is 'TestType', then test that no constructors or
                // assignments were called and the expected number of
                // destructors were called.
                if (bsl::is_same<TYPE, TestType>::value) {
                    ASSERTV(SPEC, res_spec, CTORS_AFTER  == CTORS_BEFORE);
                    ASSERTV(SPEC, res_spec, ASSIGN_AFTER == ASSIGN_BEFORE);
                    ASSERTV(SPEC, res_spec,
                            DTORS_AFTER == DTORS_BEFORE + (LEN-res_len));
                }

            } // end for (mask)
        } // end for (i)
    } // end for (op)
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test24_splice()
{
    // ------------------------------------------------------------------------
    // TESTING SPLICE
    //
    // Concerns:
    //: 1 Can splice into any position within target list.
    //:
    //: 2 Can splice from any position within source list.
    //:
    //: 3 No iterators or pointers are invalidated.
    //:
    //: 4 No allocations or deallocations occur.
    //:
    //: 5 No constructor calls, destructor calls, or assignments occur.
    //
    // Test plan:
    //: 1 Perform a small area test with source and target lists of 0 to 5
    //:   elements each, splicing into every target position from every source
    //:   position and every source length.
    //:
    //: 2 Keep track of the original iterators and element addresses from each
    //:   list and verify that they remain valid and point to the correct
    //:   element in the post-splice lists.
    //:
    //: 3 Query the number of allocations, deallocations, constructor calls,
    //:   destructor calls, and assignment operator calls before and after each
    //:   splice and verify that they do not change.
    //
    // Testing:
    //   void splice(iterator pos, list& other);
    //   void splice(iterator pos, list&& other);
    //   void splice(iterator pos, list& other, iterator i);
    //   void splice(iterator pos, list&& other, iterator i);
    //   void splice(iter pos, list& other, iter first, iter last);
    //   void splice(iter pos, list&& other, iter first, iter last);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    const char* const SPECS[] = {
        "", "A", "AB", "ABC", "ABCD", "ABCDE"
    };

    enum { NUM_SPECS = sizeof(SPECS) / sizeof(SPECS[0]) };
    const int MAX_SPEC_LEN = 5;

    enum {
        OP_FIRST,
        OP_SPLICE_ALL = OP_FIRST, // splice(pos, other&)
        OP_SPLICE_MOVE_ALL,       // splice(pos, other&&)
        OP_SPLICE_1,              // splice(pos, other&, i)
        OP_SPLICE_MOVE_1,         // splice(pos, other&&, i)
        OP_SPLICE_RANGE,          // splice(pos, other&, first, last)
        OP_SPLICE_MOVE_RANGE,     // splice(pos, other&&, first, last)
        OP_LAST
    };

    for (int op = OP_FIRST; op < OP_LAST; ++op) {
        if (verbose) {
            switch (op) {
              case OP_SPLICE_ALL: {
                printf("Testing splice(pos, other&)\n");
              } break;
              case OP_SPLICE_MOVE_ALL: {
                printf("Testing splice(pos, other&&)\n");
              } break;
              case OP_SPLICE_1: {
                printf("Testing splice(pos, other&, i)\n");
              } break;
              case OP_SPLICE_MOVE_1: {
                printf("Testing splice(pos, other&&, i)\n");
              } break;
              case OP_SPLICE_RANGE: {
                printf("Testing splice(pos, other&, first, last)\n");
              } break;
              case OP_SPLICE_MOVE_RANGE: {
                printf("Testing splice(pos, other&&, first, last)\n");
              } break;
              default: {
                ASSERT(0);
              }
            }
        }

        for (int i = 0; i < NUM_SPECS * NUM_SPECS; ++i) {
            const char* const X_SPEC = SPECS[i / NUM_SPECS ];
            const int         X_LEN  = static_cast<int>(std::strlen(X_SPEC));
            const char* const Y_SPEC = SPECS[i % NUM_SPECS ];
            const int         Y_LEN  = static_cast<int>(std::strlen(Y_SPEC));

            if (veryVerbose) { P_(X_SPEC); P(Y_SPEC); }

            ASSERTV(X_SPEC, X_LEN <= MAX_SPEC_LEN);
            ASSERTV(Y_SPEC, Y_LEN <= MAX_SPEC_LEN);

            int max_y_pos = MAX_SPEC_LEN;
            int min_y_count = 0, max_y_count = MAX_SPEC_LEN;

            switch (op) {
              case OP_SPLICE_ALL:        min_y_count = Y_LEN;           break;
              case OP_SPLICE_MOVE_ALL:   min_y_count = Y_LEN;           break;
              case OP_SPLICE_1:          min_y_count = max_y_count = 1; break;
              case OP_SPLICE_MOVE_1:     min_y_count = max_y_count = 1; break;
              case OP_SPLICE_RANGE:                                     break;
              case OP_SPLICE_MOVE_RANGE:                                break;
              default:                   ASSERT(0);                     break;
            }

            if (max_y_pos + min_y_count > Y_LEN) {
                max_y_pos = Y_LEN - min_y_count;
            }

            for (int x_pos = 0; x_pos <= X_LEN; ++x_pos) {
                for (int y_pos = 0; y_pos <= max_y_pos; ++y_pos) {
                    for (int y_count = min_y_count;
                         y_count <= Y_LEN-y_pos && y_count <= max_y_count;
                         ++y_count)
                    {
                        Obj        mX(xoa);
                        const Obj& X = gg(&mX, X_SPEC);

                        Obj        mY(xoa);
                        const Obj& Y = gg(&mY, Y_SPEC);

                        if (veryVeryVerbose) {
                            T_; P_(x_pos); P_(y_pos); P(y_count);
                            T_; T_; printf("Before: "); P_(X); P(Y);
                        }

                        // iterators and pointers to elements -- BEFORE
                        iterator    BX_iters[MAX_SPEC_LEN + 1];
                        const TYPE* BX_ptrs[MAX_SPEC_LEN];
                        iterator    BY_iters[MAX_SPEC_LEN + 1];
                        const TYPE* BY_ptrs[MAX_SPEC_LEN];

                        // iterators and pointers to elements -- AFTER
                        iterator    AX_iters[2*MAX_SPEC_LEN + 1];
                        const TYPE* AX_ptrs[2*MAX_SPEC_LEN];
                        iterator    AY_iters[MAX_SPEC_LEN + 1];
                        const TYPE* AY_ptrs[MAX_SPEC_LEN];

                        // Save iterators and pointers into BEFORE arrays
                        iterator xi = mX.begin();
                        for (int j = 0; j < X_LEN; ++j, ++xi) {
                            BX_iters[j] = xi;
                            BX_ptrs[j] = bsls::Util::addressOf(*xi);
                        }
                        BX_iters[X_LEN] = xi;

                        iterator yi = mY.begin();
                        for (int j = 0; j < Y_LEN; ++j, ++yi) {
                            BY_iters[j] = yi;
                            BY_ptrs[j] = bsls::Util::addressOf(*yi);
                        }
                        BY_iters[Y_LEN] = yi;

                        // Compute iterators and pointers AFTER splice
                        for (int j = 0; j < x_pos; ++j) {
                            AX_iters[j] = BX_iters[j];
                            AX_ptrs[j] = BX_ptrs[j];
                        }
                        for (int j = 0; j < y_pos; ++j) {
                            AY_iters[j] = BY_iters[j];
                            AY_ptrs[j] = BY_ptrs[j];
                        }
                        for (int j = 0; j < y_count; ++j) {
                            AX_iters[x_pos + j] = BY_iters[y_pos + j];
                            AX_ptrs[x_pos + j] = BY_ptrs[y_pos + j];
                        }
                        for (int j = x_pos; j < X_LEN; ++j) {
                            AX_iters[j + y_count] = BX_iters[j];
                            AX_ptrs[j + y_count]  = BX_ptrs[j];
                        }
                        AX_iters[X_LEN + y_count] = BX_iters[X_LEN];
                        for (int j = y_pos + y_count; j < Y_LEN; ++j) {
                            AY_iters[j - y_count] = BY_iters[j];
                            AY_ptrs[j - y_count]  = BY_ptrs[j];
                        }
                        AY_iters[Y_LEN - y_count] = BY_iters[Y_LEN];

                        const Int64 BB = oa.numBlocksTotal();
                        const Int64 B  = oa.numBlocksInUse();

                        const int CTORS_BEFORE = (numDefaultCtorCalls +
                                                  numIntCtorCalls     +
                                                  numCopyCtorCalls);
                        const int ASSIGN_BEFORE = numAssignmentCalls;
                        const int DTORS_BEFORE  = numDestructorCalls;

                        switch (op) {
                          case OP_SPLICE_ALL: {
                            ASSERT(0 == y_pos);
                            ASSERT(Y_LEN == y_count);
                            mX.splice(BX_iters[x_pos], mY);
                          } break;
                          case OP_SPLICE_MOVE_ALL: {
                            ASSERT(0 == y_pos);
                            ASSERT(Y_LEN == y_count);
                            mX.splice(BX_iters[x_pos], MoveUtil::move(mY));
                          } break;
                          case OP_SPLICE_1: {
                            ASSERT(1 == y_count);
                            mX.splice(BX_iters[x_pos], mY,
                                      BY_iters[y_pos]);
                          } break;
                          case OP_SPLICE_MOVE_1: {
                            ASSERT(1 == y_count);
                            mX.splice(BX_iters[x_pos], MoveUtil::move(mY),
                                      BY_iters[y_pos]);
                          } break;
                          case OP_SPLICE_RANGE: {
                            mX.splice(BX_iters[x_pos], mY,
                                      BY_iters[y_pos],
                                      BY_iters[y_pos + y_count]);
                          } break;
                          case OP_SPLICE_MOVE_RANGE: {
                            mX.splice(BX_iters[x_pos], MoveUtil::move(mY),
                                      BY_iters[y_pos],
                                      BY_iters[y_pos + y_count]);
                          } break;
                          default: {
                            ASSERT(0);
                          } break;
                        }

                        if (veryVeryVerbose) {
                            T_; T_; printf("After: "); P_(X); P(Y);
                        }

                        const Int64 AA = oa.numBlocksTotal();
                        const Int64 A  = oa.numBlocksInUse();

                        const int CTORS_AFTER = (numDefaultCtorCalls +
                                                 numIntCtorCalls     +
                                                 numCopyCtorCalls);
                        const int ASSIGN_AFTER = numAssignmentCalls;
                        const int DTORS_AFTER  = numDestructorCalls;

                        ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos, AA == BB);
                        ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos, A  == B );
                        ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos,
                                CTORS_AFTER  == CTORS_BEFORE);
                        ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos,
                                ASSIGN_AFTER == ASSIGN_BEFORE);
                        ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos,
                                DTORS_AFTER  == DTORS_BEFORE);

                        ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos,
                                checkIntegrity(X, X_LEN + y_count));

                        xi = mX.begin();
                        for (int j = 0; j < X_LEN + y_count; ++j, ++xi) {
                            ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos,
                                    AX_iters[j] == xi);
                            ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos,
                                    AX_ptrs[j] == bsls::Util::addressOf(*xi));
                        }
                        ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos, X.end() == xi);
                        ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos,
                                AX_iters[X_LEN + y_count] == xi);

                        yi = mY.begin();
                        for (int j = 0; j < Y_LEN - y_count; ++j, ++yi) {
                            ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos,
                                    AY_iters[j] == yi);
                            ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos,
                                    AY_ptrs[j] == bsls::Util::addressOf(*yi));
                        }
                        ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos, Y.end() == yi);
                        ASSERTV(X_SPEC, Y_SPEC, x_pos, y_pos,
                                AY_iters[Y_LEN - y_count] == yi);
                    } // end for (y_count)
                } // end for (y_pos)
            } // end for (x_pos)
        } // end for (i)
    } // end for (op)
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test23_reverse()
{
    // ------------------------------------------------------------------------
    // TESTING REVERSE:
    //
    // Concerns:
    //: 1 Reversing a list produced the correct result with 0, 1, 2, or more
    //:   elements.
    //:
    //: 2 Reversing a list with duplicate elements works as expected.
    //:
    //: 3 No constructors, destructors, or assignment operators of contained
    //:   elements are called.
    //:
    //: 4 No memory is allocated or deallocated.
    //
    // Plan:
    //: 1 Create a list from a variety of specifications, including empty
    //:   lists, lists of different lengths, and lists with consecutive or
    //:   non-consecutive duplicate elements and call 'reverse' on the list.
    //:
    //: 2 For concerns 1 and 2, verify that calling 'reverse' produces the
    //:   expected result.
    //:
    //: 3 For concern 3, compare the counts of 'TestType' constructors and
    //:   destructors before and after calling 'reverse' and verify that they
    //:   do not change.
    //:
    //: 4 For concern 4, use a test allocator and compare the counts of total
    //:   blocks allocated and blocks in use before and after calling 'reverse'
    //:   and verify that the counts do not change.
    //
    // Testing:
    //   void reverse();
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    struct {
        int         d_line;
        const char* d_specBefore_p;
        const char* d_specAfter_p;
    } const DATA[] = {
        { L_, "",           ""              },
        { L_, "A",          "A"             },
        { L_, "AB",         "BA"            },
        { L_, "ABC",        "CBA"           },
        { L_, "ABCD",       "DCBA"          },
        { L_, "ABBC",       "CBBA"          },
        { L_, "ABCA",       "ACBA"          },
        { L_, "AAAA",       "AAAA"          },
        { L_, "ABCDEABCD",  "DCBAEDCBA"     },
    };

    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    for (int i = 0; i < NUM_DATA; ++i) {
        const int     LINE        = DATA[i].d_line;
        const char   *SPEC_BEFORE = DATA[i].d_specBefore_p;
        const char   *SPEC_AFTER  = DATA[i].d_specAfter_p;
        const size_t  LENGTH      = std::strlen(SPEC_BEFORE);

        Obj        mX(xoa);
        const Obj& X = gg(&mX, SPEC_BEFORE);

        Obj        mExp;
        const Obj& EXP = gg(&mExp, SPEC_AFTER);

        const Int64 BB = oa.numBlocksTotal();
        const Int64 B  = oa.numBlocksInUse();

        const int CTORS_BEFORE = (numDefaultCtorCalls +
                                  numIntCtorCalls     +
                                  numCopyCtorCalls);
        const int ASSIGN_BEFORE = numAssignmentCalls;
        const int DTORS_BEFORE  = numDestructorCalls;

        mX.reverse();

        const Int64 AA = oa.numBlocksTotal();
        const Int64 A  = oa.numBlocksInUse();

        const int CTORS_AFTER = (numDefaultCtorCalls +
                                 numIntCtorCalls     +
                                 numCopyCtorCalls);
        const int ASSIGN_AFTER = numAssignmentCalls;
        const int DTORS_AFTER  = numDestructorCalls;

        ASSERTV(LINE, checkIntegrity(X, LENGTH));
        ASSERTV(LINE, EXP == X);
        ASSERTV(LINE, AA == BB);
        ASSERTV(LINE, A  == B );
        ASSERTV(LINE, CTORS_AFTER  == CTORS_BEFORE);
        ASSERTV(LINE, ASSIGN_AFTER == ASSIGN_BEFORE);
        ASSERTV(LINE, DTORS_AFTER  == DTORS_BEFORE);
    } // end for (i)
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test22_typeTraits()
{
    // ------------------------------------------------------------------------
    // TESTING TYPE TRAITS:
    //
    // Concerns:
    //: 1 That the list has the 'bslalg::HasStlIterators' trait.
    //:
    //: 2 If instantiated with 'bsl::allocator', then list has the
    //:   'bslma::UsesBslmaAllocator' trait.
    //:
    //: 3 If instantiated with an allocator that is bitwise movable, then the
    //:   list has the 'bslmf::IsBitwiseMoveable' trait.
    //:
    //: 4 That a list never has the 'bsl::is_trivially_copyable' trait.
    //
    // Plan:
    //: 1 Test each of the above four traits and compare their values to the
    //:   expected values.
    //
    // Testing:
    //   bslalg::HasStlIterators
    //   bslma::UsesBslmaAllocator
    //   bslmf::IsBitwiseMoveable
    //   bsl::is_trivially_copyable
    // ------------------------------------------------------------------------

    BSLMF_ASSERT(bslalg::HasStlIterators<Obj>::value);

    BSLMF_ASSERT((bsl::is_same<ALLOC, bslma::Allocator *>::value ||
                  bsl::is_same<ALLOC, bsl::allocator<TYPE> >::value) ==
                                        bslma::UsesBslmaAllocator<Obj>::value);

    BSLMF_ASSERT(bslmf::IsBitwiseMoveable<Obj>::value
                                    == bslmf::IsBitwiseMoveable<ALLOC>::value);

    BSLMF_ASSERT(! bsl::is_trivially_copyable<Obj>::value);
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test21_typedefs()
{
    // ------------------------------------------------------------------------
    // TESTING TYPEDEFS:
    //
    // Concerns:
    //: 1 That all of the required typedefs are defined.
    //:
    //: 2 That the typedefs are identical to the corresponding typedefs from
    //:   the allocator.
    //
    // Plan:
    //: 1 Use compile-time asserts calling the 'bsl::is_same' template to test
    //:   that various types match.  Note that the iterator types were tested
    //:   in test case 16 and so are not tested here.
    //
    // Testing:
    //   reference
    //   const_reference
    //   size_type
    //   difference_type
    //   value_type
    //   allocator_type
    //   pointer
    //   const_pointer
    // ------------------------------------------------------------------------

    BSLMF_ASSERT((bsl::is_same<typename ALLOC::reference,
                               typename Obj::reference>::value));
    BSLMF_ASSERT((bsl::is_same<typename ALLOC::const_reference,
                               typename Obj::const_reference>::value));

    BSLMF_ASSERT((bsl::is_same<typename ALLOC::pointer,
                               typename Obj::pointer>::value));
    BSLMF_ASSERT((bsl::is_same<TYPE *,
                               typename Obj::pointer>::value));
    BSLMF_ASSERT((bsl::is_same<typename ALLOC::const_pointer,
                               typename Obj::const_pointer>::value));
    BSLMF_ASSERT((bsl::is_same<const TYPE *,
                               typename Obj::const_pointer>::value));

    BSLMF_ASSERT((bsl::is_same<typename ALLOC::size_type,
                               typename Obj::size_type>::value));
    BSLMF_ASSERT((bsl::is_same<typename ALLOC::difference_type,
                               typename Obj::difference_type>::value));

    BSLMF_ASSERT((bsl::is_same<TYPE, typename Obj::value_type>::value));
    BSLMF_ASSERT((bsl::is_same<ALLOC, typename Obj::allocator_type>::value));
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test20_comparisonOps(bsl::true_type)
{
    // ------------------------------------------------------------------------
    // TESTING COMPARISON FREE OPERATORS:
    //
    // Concerns:
    //: 1 'operator<' returns the lexicographic comparison on two lists.
    //:
    //: 2 'operator>', 'operator<=', and 'operator>=' are correctly tied to
    //:   'operator<'.
    //:
    //: 3 'operator<=>' is consistent with '<', '>', '<=', '>='.
    //:
    //: 4 That traits get selected properly.
    //
    // Plan:
    //: 1 For a variety of lists of different sizes and different values, test
    //:   that the comparison returns as expected.
    //
    // Testing:
    //   bool operator<(const list<T,A>&, const list<T,A>&);
    //   bool operator>(const list<T,A>&, const list<T,A>&);
    //   bool operator<=(const list<T,A>&, const list<T,A>&);
    //   bool operator>=(const list<T,A>&, const list<T,A>&);
    //   auto operator<=>(const list&, const list&);
    // ------------------------------------------------------------------------

    // NOTE: These specs must be sorted in lexicographical order
    static const char *SPECS[] = {
        "",
        "A",
        "AA",
        "AAA",
        "AAAA",
        "AAAAA",
        "AAAAAA",
        "AAAAAAA",
        "AAAAAAAA",
        "AAAAAAAAA",
        "AAAAAAAAAA",
        "AAAAAAAAAAA",
        "AAAAAAAAAAAA",
        "AAAAAAAAAAAAA",
        "AAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAA",
        "AAAAAB",
        "AAAAABA",
        "AAAAABAA",
        "AAAAABAAA",
        "AAAAABAAAA",
        "AAAAABAAAAA",
        "AAAAABAAAAAA",
        "AAAAABAAAAAAA",
        "AAAAABAAAAAAAA",
        "AAAAABAAAAAAAAA",
        "AAAAB",
        "AAAABAAAAAA",
        "AAAABAAAAAAA",
        "AAAABAAAAAAAA",
        "AAAABAAAAAAAAA",
        "AAAABAAAAAAAAAA",
        "AAAB",
        "AAABA",
        "AAABAA",
        "AAABAAAAAA",
        "AAB",
        "AABA",
        "AABAA",
        "AABAAA",
        "AABAAAAAA",
        "AB",
        "ABA",
        "ABAA",
        "ABAAA",
        "ABAAAAAA",
        "B",
        "BA",
        "BAA",
        "BAAA",
        "BAAAA",
        "BAAAAA",
        "BAAAAAA",
        "BB",
    };

    enum { NUM_SPECS = sizeof(SPECS) / sizeof(SPECS[0]) };

    if (verbose) printf("Compare each pair of similar and different"
                        " values (u, v) in S x S \n.");
    {
        // Create first object
        for (int si = 0; si < NUM_SPECS; ++si) {
            const char *const U_SPEC = SPECS[si];

            Obj mU;  const Obj& U = gg(&mU, U_SPEC);

            if (veryVerbose) {
                T_; T_; P_(U_SPEC); P(U);
            }

            // Create second object
            for (int sj = 0; sj < NUM_SPECS; ++sj) {
                const char *const V_SPEC = SPECS[sj];

                Obj mV;  const Obj& V = gg(&mV, V_SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P_(V_SPEC); P(V);
                }

                const bool isLT = si <  sj;
                const bool isLE = si <= sj;
                const bool isGT = si >  sj;
                const bool isGE = si >= sj;

                ASSERTV(si, sj, isLT == (U <  V));
                ASSERTV(si, sj, isLE == (U <= V));
                ASSERTV(si, sj, isGT == (U >  V));
                ASSERTV(si, sj, isGE == (U >= V));
                ASSERTV(si, sj, (U <  V) == !(U >= V));
                ASSERTV(si, sj, (U >  V) == !(U <= V));
#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
                const auto cmp = U <=> V;
                ASSERTV(si, sj, isLT == (cmp <  0));
                ASSERTV(si, sj, isLE == (cmp <= 0));
                ASSERTV(si, sj, isGT == (cmp >  0));
                ASSERTV(si, sj, isGE == (cmp >= 0));
#endif
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test20_comparisonOps(bsl::false_type)
{
    // no-op.  'TYPE' has no 'operator<'.
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test20_comparisonOps()
{
    test20_comparisonOps(HasOperatorLessThan());
}

template <class TYPE, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
          bool OTHER_FLAGS>
void TestDriver2<TYPE, ALLOC>::test19_propagate_on_container_swap_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<TYPE,
                                        OTHER_FLAGS,
                                        OTHER_FLAGS,
                                        PROPAGATE_ON_CONTAINER_SWAP_FLAG,
                                        OTHER_FLAGS> StdAlloc;

    typedef bsl::list<TYPE, StdAlloc>                Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_SWAP_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const ISPEC   = SPECS[ti];

        TestValues IVALUES(ISPEC);

        bslma::TestAllocator xoa("x-original", veryVeryVeryVerbose);
        bslma::TestAllocator yoa("y-original", veryVeryVeryVerbose);

        StdAlloc xma(&xoa);
        StdAlloc yma(&yoa);

        StdAlloc scratch(&da);

        const Obj ZZ(IVALUES.begin(), IVALUES.end(), scratch);  // control

        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            const Obj WW(JVALUES.begin(), JVALUES.end(), scratch);  // control

            {
                IVALUES.resetIterators();

                Obj        mX(IVALUES.begin(), IVALUES.end(), xma);
                const Obj& X = mX;

                if (veryVerbose) { T_ P_(ISPEC) P_(X) P(ZZ) }

                JVALUES.resetIterators();

                Obj        mY(JVALUES.begin(), JVALUES.end(), yma);
                const Obj& Y = mY;

                ASSERTV(ISPEC, JSPEC, ZZ, X, ZZ == X);
                ASSERTV(ISPEC, JSPEC, WW, Y, WW == Y);

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor dam(&da);
                    bslma::TestAllocatorMonitor xoam(&xoa);
                    bslma::TestAllocatorMonitor yoam(&yoa);

                    mX.swap(mY);

                    ASSERTV(ISPEC, JSPEC, WW, X, WW == X);
                    ASSERTV(ISPEC, JSPEC, ZZ, Y, ZZ == Y);

                    if (PROPAGATE) {
                        ASSERTV(ISPEC, JSPEC, yma == X.get_allocator());
                        ASSERTV(ISPEC, JSPEC, xma == Y.get_allocator());

                        ASSERTV(ISPEC, JSPEC, dam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, xoam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, yoam.isTotalSame());
                    }
                    else {
                        ASSERTV(ISPEC, JSPEC, xma == X.get_allocator());
                        ASSERTV(ISPEC, JSPEC, yma == Y.get_allocator());
                    }
                }

                // free function 'swap'
                {
                    bslma::TestAllocatorMonitor dam(&da);
                    bslma::TestAllocatorMonitor xoam(&xoa);
                    bslma::TestAllocatorMonitor yoam(&yoa);

                    swap(mX, mY);

                    ASSERTV(ISPEC, JSPEC, ZZ, X, ZZ == X);
                    ASSERTV(ISPEC, JSPEC, WW, Y, WW == Y);

                    ASSERTV(ISPEC, JSPEC, xma == X.get_allocator());
                    ASSERTV(ISPEC, JSPEC, yma == Y.get_allocator());

                    if (PROPAGATE) {
                        ASSERTV(ISPEC, JSPEC, dam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, xoam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, yoam.isTotalSame());
                    }
                }
            }
            ASSERTV(ISPEC, 0 == xoa.numBlocksInUse());
            ASSERTV(ISPEC, 0 == yoa.numBlocksInUse());
        }
    }
    ASSERTV(0 == da.numBlocksInUse());
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::test19_propagate_on_container_swap()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_swap' trait is 'false', the
    //:   allocators used by the source and target objects remain unchanged
    //:   (i.e., the allocators are *not* exchanged).
    //:
    //: 2 If the 'propagate_on_container_swap' trait is 'true', the
    //:   allocator used by the target (source) object is updated to be a copy
    //:   of that used by the source (target) object (i.e., the allocators
    //:   *are* exchanged).
    //:
    //: 3 If the allocators are propagated (i.e., exchanged), there is no
    //:   additional allocation from any allocator.
    //:
    //: 4 The effect of the 'propagate_on_container_swap' trait is independent
    //:   of the other three allocator propagation traits.
    //:
    //: 5 Following the swap operation, neither object holds on to memory
    //:   allocated from the other object's allocator.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_swap' property configured to 'false'.  In two
    //:   successive iterations of P-3, first configure the three properties
    //:   not under test to be 'false', then configure them all to be 'true'.
    //:
    //: 3 For each value '(x, y)' in the cross product S x S:  (C-1)
    //:
    //:   1 Initialize two objects from 'x', a control object 'ZZ' using a
    //:     scratch allocator and an object 'X' using one of the allocators
    //:     from P-2.
    //:
    //:   2 Initialize two objects from 'y', a control object 'WW' using a
    //:     scratch allocator and an object 'Y' using the other allocator from
    //:     P-2.
    //:
    //:   3 Using both member 'swap' and free function 'swap', swap 'X' with
    //:     'Y' and use 'operator==' to verify that 'X' and 'Y' have the
    //:     expected values.
    //:
    //:   4 Use the 'get_allocator' method to verify that the allocators of 'X'
    //:     and 'Y' are *not* exchanged.  (C-1)
    //:
    //: 4 Repeat P-2..3 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocators of 'X' and 'Y'
    //:   *are* exchanged.  Also verify that there is no additional allocation
    //:   from any allocator.  (C-2..5)
    //
    // Testing:
    //   propagate_on_container_swap
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nSWAP MEMBER AND FREE FUNCTIONS: ALLOCATOR PROPAGATION"
               "\n=====================================================\n");

    if (verbose) printf("\n'propagate_on_container_swap::value == false'\n");

    test19_propagate_on_container_swap_dispatch<false, false>();
    test19_propagate_on_container_swap_dispatch<false, true>();

    if (verbose) printf("\n'propagate_on_container_swap::value == true'\n");

    test19_propagate_on_container_swap_dispatch<true, false>();
    test19_propagate_on_container_swap_dispatch<true, true>();
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test19_swap_noexcept()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS: NOEXCEPT SPECIFICATIONS
    //
    // Concerns:
    //: 1 If 'allocator_traits<Allocator>::is_always_equal::value' is
    //:   true, the 'swap' functions are 'noexcept(true)'.
    //
    // Plan:
    //: 1 Compare the value of the trait with the member 'swap' function
    //:   noexcept specification.
    //:
    //: 2 Compare the value of the trait with the free 'swap' function noexcept
    //:   specification.
    //
    // Testing:
    //   list::swap()
    //   swap(list& , list& )
    // ------------------------------------------------------------------------

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    bsl::list<TYPE, ALLOC> a, b;

    const bool isNoexcept = AllocTraits::is_always_equal::value;
    ASSERT(isNoexcept == BSLS_KEYWORD_NOEXCEPT_OPERATOR(a.swap(b)));
    ASSERT(isNoexcept == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(a,b)));
#endif
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test19_swap()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS
    //   Ensure that, when member and free 'swap' are implemented, we can
    //   exchange the values of any two containers of the same type.
    //
    // Concerns:
    //: 1 Both functions exchange the values of the (two) supplied objects.
    //:
    //: 2 Both functions have standard signatures and return types.
    //:
    //: 3 Using either function to swap an object with itself does not
    //:   affect the value of the object (alias-safety).
    //:
    //: 4 If the two objects being swapped use the same allocator, neither
    //:   function allocates memory from any allocator and the allocator
    //:   address held by both objects is unchanged.
    //:
    //: 5 If the two objects being swapped use different allocators, then both
    //:   allocators may allocate memory and the allocators held by both
    //:   objects are unchanged.
    //:
    //: 6 Both functions provide the basic exception guarantee w.r.t. to
    //:   memory allocation.
    //:
    //: 7 The free 'swap' function is discoverable through ADL (Argument
    //:   Dependent Lookup), and through the standard C++ idiom for calling
    //:   'swap'.
    //:
    //: 8 Neither 'swap' function uses the default allocator, unless one of the
    //:   objects swapped uses the default allocator.
    //:
    //: 9 While the 'swap' functions may allocate memory, they free as much
    //:   memory as they allocate for no net change in memory in use, provided
    //:   both allocators are of type 'bslma::TestAllocator'.
    //:
    //:10 If 'allocator_traits<Allocator>::is_always_equal::value' is
    //:   true, the 'swap' functions are 'noexcept(true)'.
    //
    // Plan:
    //: 1 Use the addresses of the 'swap' member and free functions defined
    //:   in this component to initialize, respectively, member-function
    //:   and free-function pointers having the appropriate signatures and
    //:   return types.  (C-2)
    //:
    //: 2 Repeat the following run-time tests in this test doing swaps using
    //:   each of the following four alternatives.  The behavior should be
    //:   identical in each case.  Note that for the types of swap other than
    //:   'member swap', the test need only be performed once.
    //:
    //:   1 By calling free 'swap', no namespace specified.  The only namespace
    //:     in scope when this 'swap' is called is 'BloombergLP', and there is
    //:     no 'swap' in that namespace.
    //:
    //:   2 By calling member 'swap'.
    //:
    //:   3 By invoking 'swap' using ADL lookup by calling the 'invokeAdlSwap'
    //:     helper: (C-7)
    //:
    //:     o The 'invokeAdlSwap' helper function will call free 'swap' after
    //:       'using' the namespace 'incorrect', which contains a matching
    //:       function 'incorrect::swap' designed to fail.  If ADL lookup
    //:       functions correctly, the correct function 'bsl::swap(Obj&, Obj&)'
    //:       will be preferred and the test will pass.
    //:
    //:   4 By invoking 'swap' using the standard C++ idiom by calling the
    //:     'invokePatternSwap' helper: (C-7)
    //:
    //:     o The 'invokePatternSwap' helper function will call free 'swap'
    //:       after 'using' the namespace 'bsl'.  This is the standard idiom
    //:       that BDE clients are expected to use.
    //:
    //: 3 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).  Regularly query before and after swaps to
    //:   verify that none of the swaps used the default allocator.
    //:
    //: 4 Using the table-driven technique:
    //:
    //:   o Specify a set of (unique) valid object values (one per row), of
    //:     increasing object 'size', where the first row in the table should
    //:     specify a value equal to that of a default-constructed object.
    //:
    //: 5 For each row 'R1' in the table of P-3: (C-1, 3..6)
    //:
    //:   1 Create two 'bslma::TestAllocator' objects, 'oa' and 'scratch'.
    //:
    //:   2 Use the default constructor and 'oa' to create a modifiable 'Obj',
    //:     'mW', and use the 'gg' function to give it the value described by
    //:     'R1'; use the same method to create an identical object 'mXX' using
    //:     the 'scratch' allocator.  Create 'const' references 'W' and 'XX'
    //:     from 'mW' and 'mXX', respectively.
    //:
    //:   3 Swap the value of 'mW' with itself; verify, after each swap,
    //:     that: (C-3..4)
    //:
    //:     1 The value is unchanged.  (C-3)
    //:
    //:     2 The allocator held by the object is unchanged.  (C-4)
    //:
    //:     3 There was no additional object memory allocation.  (C-4)
    //:
    //:   4 For each row 'R2' in the table of P-3: (C-1, 4, 7)
    //:
    //:     1 Use the default constructor, 'oa', and the 'gg' function to
    //:       create a modifiable 'Obj', 'mX', identical to 'mXX'.  Base a
    //:       'const' reference 'X' off of 'mX'.  (P-4.2).
    //:
    //:     2 Use the default constructor and the 'gg' function to create a
    //:       modifiable 'Obj' 'mY' having the value described by 'R2', using
    //:       'oa', and use the same technique to create an identical object
    //:       'mYY'.  Create 'const' references 'Y' and 'YY' based on 'mY' and
    //:       'mYY', respectively.
    //:
    //:     3 Swap the values of 'mX' and 'mY' using both member and free swap;
    //:       verify, after each swap, that: (C-1, 4)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The default allocator was not used during the swap.  (C-8)
    //:
    //:       3 The common object allocator address held by 'mX' and 'mY'
    //:         is unchanged in both objects.  (C-4)
    //:
    //:       4 There was no additional object memory allocation.  (C-4)
    //:
    //:     4 Create a new object allocator, 'oaz'.
    //:
    //:     5 Use the default constructor, 'oa', and the 'gg' function to
    //:       create a modifiable 'Obj' 'mA', having the value described by
    //:       'R1'.  Use the default constructor, 'oaz', and the 'gg' function
    //:       to create a modifiable 'Obj' 'mB', having the value described by
    //:       'R2'.
    //:
    //:     6 Use the member and free 'swap' functions to swap the values of
    //:       'mA' and 'mB', respectively, under the presence of exceptions;
    //:       verify, after each swap, that: (C-1, 5, 6, 8, 9)
    //:
    //:       1 If no exception occurred:
    //:
    //:         1 The values have been exchanged. (C-1)
    //:
    //:         2 The default allocator was not used during the swap.  (C-8)
    //:
    //:         3 The total amount of memory in use by the two allocators is
    //:           unchanged.  (C-9)
    //:
    //:     7 For one pair of values, swap 'mX' and 'mY' using both
    //:       'invokePatternSwap' and 'invokeAdlSwap'.  Verify, after each
    //:       swap, that: (C-1, 4)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The default allocator was not used during the swap.  (C-8)
    //:
    //:       3 The common object allocator address held by 'mX' and 'mY'
    //:         is unchanged in both objects.  (C-4)
    //:
    //:       4 There was no additional object memory allocation.  (C-4)
    //:
    //: 6 To address concern 10, pass allocators with both 'is_always_equal'
    //:   values (true & false).
    //
    // Testing:
    //   void swap(Obj& rhs);
    //   void bsl::swap(Obj& lhs, Obj& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("SWAP MEMBER AND FREE FUNCTIONS: %s\n"
                        "==============================\n",
                        bsls::NameOf<TYPE>().name());

    // Verify that the signatures and return types are standard.

    {
        typedef void (Obj::*FuncPtr)(Obj&);
        typedef void (*FreeFuncPtr)(Obj&, Obj&);

        FuncPtr     memberSwap = &Obj::swap;
        FreeFuncPtr freeSwap   = &bsl::swap;

        (void)memberSwap;  // quash potential compiler warnings
        (void)freeSwap;
    }

    // Set up the default allocator.

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    // Use the default table.

    enum {                 NUM_DATA        = DEFAULT_NUM_DATA,
                           LAST_SPEC       = NUM_DATA - 1 };
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    ALLOC                xscratch(&scratch);

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char *const SPEC1 = DATA[ti].d_spec_p;

        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
        ALLOC                xoa(&oa);

        Obj mW(xoa);        const Obj& W  = gg(&mW,  SPEC1);
        Obj mXX(xscratch);  const Obj& XX = gg(&mXX, SPEC1);
        ASSERT(W == XX);

        if (veryVerbose) { T_ P_(SPEC1) P_(W) P(XX) }

        // Ensure the first row of the table contains the default-constructed
        // value.

        static bool firstFlag = true;
        if (firstFlag) {
            firstFlag = false;
            ASSERTV(SPEC1, Obj(), W, Obj() == W);
        }

        for (int freeSwap = 0; freeSwap < 2; ++freeSwap) {
            bslma::TestAllocatorMonitor oam(&oa);
            bslma::TestAllocatorMonitor dam(&da);

            if (freeSwap) {
                swap(mW, mW);
            }
            else {
                mW.swap(mW);
            }

            ASSERTV(SPEC1, XX, W, XX == W);
            ASSERTV(SPEC1, xoa == W.get_allocator());
            ASSERTV(SPEC1, oam.isTotalSame());
            ASSERTV(SPEC1, dam.isTotalSame());
        }
    }

    // Swap objects using the same allocator with member and free 'swap' --
    // shouldn't allocate.  Run tests over exhaustive pairs of values.

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char *const SPEC1 = DATA[ti].d_spec_p;

        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
        ALLOC                xoa(&oa);

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
        ALLOC                xscratch(&scratch);

        Obj mXX(xscratch);  const Obj& XX = gg(&mXX, SPEC1);

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const char *const SPEC2 = DATA[tj].d_spec_p;

            Obj mYY(xscratch);  const Obj& YY = gg(&mYY, SPEC2);

            ASSERT(ti == tj ? XX == YY : XX != YY);

            for (int freeSwap = 0; freeSwap < 2; ++freeSwap) {
                Obj mX(xoa);  const Obj& X  = gg(&mX,  SPEC1);

                ASSERT(X == XX);

                Obj mY(xoa);  const Obj& Y  = gg(&mY,  SPEC2);

                ASSERT(Y == YY);

                if (veryVerbose) { T_ P_(SPEC1) P(SPEC2); }

                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor dam(&da);

                if (freeSwap) {
                    swap(mX, mY);
                }
                else {
                    mX.swap(mY);
                }

                ASSERTV(SPEC1, SPEC2, YY, X, YY == X);
                ASSERTV(SPEC1, SPEC2, XX, Y, XX == Y);
                ASSERTV(SPEC1, SPEC2, xoa == X.get_allocator());
                ASSERTV(SPEC1, SPEC2, xoa == Y.get_allocator());
                ASSERTV(SPEC1, SPEC2, oam.isTotalSame());
                ASSERTV(SPEC1, SPEC2, dam.isTotalSame());
            }
        }
    }

    // Swap objects using different allocators -- may allocate memory.

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char *const SPEC1 = DATA[ti].d_spec_p;
        const size_t      LEN1  = std::strlen(SPEC1);

        if (4 < LEN1 && NUM_DATA-1 != ti) {
            continue;
        }

        Obj mXX(xscratch);  const Obj& XX = gg(&mXX, SPEC1);

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const char *const SPEC2 = DATA[tj].d_spec_p;
            const size_t      LEN2  = std::strlen(SPEC2);

            if (4 < LEN2 && NUM_DATA-1 != tj) {
                continue;
            }

            Obj mYY(xscratch);  const Obj& YY = gg(&mYY, SPEC2);

            bslma::TestAllocator aa("mA allocator", veryVeryVeryVerbose);
            ALLOC                xaa(&aa);

            bslma::TestAllocator ba("mB allocator", veryVeryVeryVerbose);
            ALLOC                xba(&ba);

            bool doneFree = false, doneBThrow = false;
            for (int mode = 0; mode < 4; ++mode) {
                bool freeSwap = mode & 1;
                bool bThrow   = mode & 2;

                freeSwap && (doneFree   = true);
                bThrow   && (doneBThrow = true);

                bslma::TestAllocator& ta = bThrow ? ba : aa;

                int numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                    const Int64 taLimit = ta.allocationLimit();
                    ta.setAllocationLimit(-1);

                    Obj mA(xaa);  const Obj& A = gg(&mA, SPEC1);
                    Obj mB(xba);  const Obj& B = gg(&mB, SPEC2);

                    ASSERT(XX == A && YY == B);
                    ASSERTV(ti == tj ? A == B && XX == YY
                                     : A != B && XX != YY);

                    bslma::TestAllocatorMonitor aam(&aa);
                    bslma::TestAllocatorMonitor bam(&ba);
                    bslma::TestAllocatorMonitor dam(&da);

                    const Int64 AU = aa.numBytesInUse() + ba.numBytesInUse();

                    ta.setAllocationLimit(taLimit);

                    ++numThrows;

                    if (freeSwap) {
                        swap(mA, mB);
                    }
                    else {
                        mA.swap(mB);
                    }

                    ASSERTV(SPEC1, SPEC2, YY, A, YY == A);
                    ASSERTV(SPEC1, SPEC2, XX, B, XX == B);

#ifdef BDE_BUILD_TARGET_EXC
                    // This check is custom for 'list' and will need to be
                    // tailored to each container type.

                    const Int64 throwSize = bThrow ? XX.size() : YY.size();
                    ASSERTV(bThrow, throwSize, numThrows,
                            throwSize + 1 <= numThrows);
#endif

                    ASSERTV(SPEC1, SPEC2, dam.isTotalSame());

                    ASSERTV(AU, aa.numBytesInUse(),  ba.numBytesInUse(),
                            aa.numBytesInUse() + ba.numBytesInUse(),
                            aa.numBytesInUse() + ba.numBytesInUse() == AU);

                    ASSERTV(SPEC1,SPEC2, xaa == A.get_allocator());
                    ASSERTV(SPEC1,SPEC2, xba == B.get_allocator());
                    ASSERTV(SPEC1,SPEC2, YY.empty() || aam.isTotalUp());
                    ASSERTV(SPEC1,SPEC2, XX.empty() || bam.isTotalUp());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
            ASSERT(doneFree && doneBThrow);
        }
    }

    // Swap objects using the same allocator once with pattern and ADL swap.
    // Shouldn't allocate.  Run tests on a single pair of values.

    for (int adlSwap = 0; adlSwap < 2; ++adlSwap) {
        const char *SPEC1 = "A";
        const char *SPEC2 = "BCDE";

        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
        ALLOC                xoa(&oa);

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
        ALLOC                xscratch(&scratch);

        Obj mX(xoa);       const Obj& X  = gg(&mX,  SPEC1);
        Obj mXX(xscratch); const Obj& XX = gg(&mXX, SPEC1);

        ASSERT(X == XX);

        Obj mY(xoa);       const Obj& Y  = gg(&mY,  SPEC2);
        Obj mYY(xscratch); const Obj& YY = gg(&mYY, SPEC2);

        ASSERT(Y == YY);

        ASSERT(XX != YY && X != Y);

        bslma::TestAllocatorMonitor oam(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        if (adlSwap) {
            invokeAdlSwap(&mX, &mY);
        }
        else {
            invokePatternSwap(&mX, &mY);
        }

        ASSERTV(SPEC1, SPEC2, YY, X, YY == X);
        ASSERTV(SPEC1, SPEC2, XX, Y, XX == Y);
        ASSERTV(SPEC1, SPEC2, xoa == X.get_allocator());
        ASSERTV(SPEC1, SPEC2, xoa == Y.get_allocator());
        ASSERTV(SPEC1, SPEC2, oam.isTotalSame());
        ASSERTV(SPEC1, SPEC2, dam.isTotalSame());
    }

    // Test noexcept specifications of the 'swap' functions.
    TestDriver2<TYPE, ALLOC>::test19_swap_noexcept();
    // is_always_equal == true
    typedef StatelessAllocator<TYPE> StatelessAlloc;
    ASSERT(bsl::allocator_traits<StatelessAlloc>::is_always_equal::value);
    TestDriver2<TYPE, StatelessAlloc>::test19_swap_noexcept();
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test18_erase()
{
    // ------------------------------------------------------------------------
    // TESTING ERASE AND POP:
    //
    // Concerns:
    //: 1 That the resulting value is correct.
    //:
    //: 2 That erase operations do not allocate memory.
    //:
    //: 3 That no memory is leaked.
    //
    // Plan:
    //: 1 For the 'erase' methods, the concerns are simply to cover the full
    //:   range of possible indices and numbers of elements.  We build a list
    //:   with a variable size and capacity, and remove a variable element or
    //:   number of elements from it, and verify that size, capacity, and value
    //:   are as expected:
    //:   o Without exceptions, and computing the number of allocations.
    //:   o That the total allocations do not increase.
    //:   o That the in-use allocations diminish by the correct amount.
    //
    // Testing:
    //   void pop_back();
    //   void pop_front();
    //   iterator erase(const_iterator pos);
    //   iterator erase(const_iterator first, const_iterator last);
    // -------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    const ALLOC          xoa(&oa);

    const int           MAX_LEN    = 15;

    // Operations to test
    enum {
        TEST_FIRST,
        TEST_ERASE1 = TEST_FIRST,     // erase(pos);
        TEST_ERASE_RANGE,             // erase(first, last);
        TEST_POP_BACK,                // pop_back();
        TEST_POP_FRONT,               // pop_front();
        TEST_LAST
    };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // container spec
    } DATA[] = {
        //line  spec                            length
        //----  ----                            ------
        { L_,   ""                        }, // 0
        { L_,   "A"                       }, // 1
        { L_,   "AB"                      }, // 2
        { L_,   "ABC"                     }, // 3
        { L_,   "ABCD"                    }, // 4
        { L_,   "ABCDA"                   }, // 5
        { L_,   "ABCDABCDABCDABC"         }, // 15
    };
    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    // Iterate through the operations
    for (int op = TEST_FIRST; op < TEST_LAST; ++op) {

        const char* opname = "<unknown>";

        switch (op) {
            case TEST_ERASE1:      opname = "erase(iterator)";           break;
            case TEST_ERASE_RANGE: opname = "erase(iterator, iterator)"; break;
            case TEST_POP_BACK:    opname = "pop_back()";                break;
            case TEST_POP_FRONT:   opname = "pop_front()";               break;
        }

        if (verbose) printf("testing %s\n", opname);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_lineNum;
            const char *SPEC   = DATA[ti].d_spec_p;
            const int   LENGTH = static_cast<int>(std::strlen(SPEC));

            ASSERTV(LENGTH, LENGTH <= MAX_LEN);

            int pos_first, pos_last;  // possible start positions

            switch (op) {
                case TEST_ERASE1:
                    pos_first = 0;
                    pos_last  = LENGTH - 1;
                    break;
                case TEST_ERASE_RANGE:
                    pos_first = 0;
                    pos_last  = LENGTH;
                    break;
                case TEST_POP_BACK:
                    pos_first = LENGTH - 1;
                    pos_last  = LENGTH - 1;
                    break;
                case TEST_POP_FRONT:
                    pos_first = 0;
                    pos_last  = 0;
                    break;
                default: // gcc static analysis requires this to avoid warnings
                    ASSERT(!"Bad operation requested for test");
                    return;                                           // RETURN
            } // end switch

            for (int posidx = pos_first; posidx <= pos_last; ++posidx) {

                int erase_min, erase_max;   // number of elements to be erased
                if (TEST_ERASE_RANGE == op) {
                    erase_min = 0;
                    erase_max = LENGTH - posidx;
                }
                else {
                    erase_min = 1;
                    erase_max = 1;
                    if (LENGTH < 1) continue;
                }

                for (int n = erase_min; n <= erase_max; ++n) {
                    Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);
                    Obj mY;       const Obj& Y = gg(&mY, SPEC);    // control

                    // Save original iterators (including end iterator) C++0x
                    // allows erasing using const_iterator

                    const_iterator orig_iters[MAX_LEN + 1];
                    const_iterator it = X.begin();
                    for (int i = 0; i < LENGTH + 1; ++i, ++it) {
                        orig_iters[i] = it;
                    }

                    // C++0x allows erasing using const_iterator

                    const_iterator pos = orig_iters[posidx];
                    iterator       ret;

                    const Int64 BB = oa.numBlocksTotal();
                    const Int64 B  = oa.numBlocksInUse();

                    if (veryVerbose) {
                        T_; P_(SPEC); P_(posidx); P(n);
                    }

                    switch (op) {
                        case TEST_ERASE1: {
                            ret = mX.erase(pos);
                        } break;
                        case TEST_ERASE_RANGE: {
                            // C++0x allows erasing using const_iterator

                            const_iterator end_range = succ(pos, n);
                            ret = mX.erase(pos, end_range);
                        } break;
                        case TEST_POP_BACK: {
                            mX.pop_back();
                            ret = mX.end();
                        } break;
                        case TEST_POP_FRONT: {
                            mX.pop_front();
                            ret = mX.begin();
                        } break;
                    } // end switch

                    // Should never have an exception, so should always get
                    // here.

                    const Int64 AA = oa.numBlocksTotal();
                    const Int64 A  = oa.numBlocksInUse();

                    // Test important values

                    ASSERTV(LINE, op, posidx, checkIntegrity(X, LENGTH - n));
                    ASSERTV(LINE, op, posidx, LENGTH - n == (int) X.size());
                    ASSERTV(LINE, op, posidx, BB == AA);
                    ASSERTV(LINE, op, posidx, B + deltaBlocks(-n) == A);

                    const_iterator cit = X.begin();
                    const_iterator yi  = Y.begin();
                    for (int i = 0; i < LENGTH; ++i, ++yi) {
                        if (i < posidx) {
                            // Test that part before erasure is unchanged
                            ASSERTV(LINE, op, posidx, i, *yi == *cit);
                            ASSERTV(LINE, op, posidx, i, orig_iters[i] == cit);
                            ++cit;
                        }
                        else if (i < posidx + n) {
                            // skip erased values
                            continue;
                        }
                        else {
                            // Test that part after erasure is unchanged
                            ASSERTV(LINE, op, posidx, i, *yi == *cit);
                            ASSERTV(LINE, op, posidx, i, orig_iters[i] == cit);
                            ++cit;
                        }
                    }
                    // Test end iterator
                    ASSERTV(LINE, op, posidx, X.end() == cit);
                    ASSERTV(LINE, op, posidx, Y.end() == yi);
                    ASSERTV(LINE, op, posidx, orig_iters[LENGTH] == cit);

                } // end for (n)

                ASSERTV(LINE, op, posidx, 0 == oa.numBlocksInUse());
            } // end for (posidx)
        } // end for (ti)
    } // end for (op)
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test17_emplace()
{
    // ------------------------------------------------------------------------
    // TESTING EMPLACE:
    //
    // Concerns
    //: 1 That the resulting list value is correct.
    //:
    //: 2 That the 'emplace' return (if any) value is a valid iterator to the
    //:   first inserted element or to the insertion position if no elements
    //:   are inserted.
    //:
    //: 3 That 'emplace' has the strong exception guarantee.
    //:
    //: 4 That 'emplace' is exception neutral w.r.t. memory allocation.
    //:
    //: 5 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 6 That inserting a 'const T& value' that is a reference to an element
    //:   of the list does not suffer from aliasing problems.
    //:
    //: 7 That no iterators are invalidated by the insertion.
    //:
    //: 8 That 'emplace' passes 0 to 5 arguments to the 'T' constructor.
    //:
    //: 9 'emplace_back' and 'emplace_front' return a reference to the inserted
    //:   element.
    //
    // Plan:
    //: 1 Create objects of various sizes and insert a distinct value into each
    //:   possible position.
    //:
    //: 2 For concerns 1, 2, 5 and 9 verify that the return value and modified
    //:   list are as expected.
    //:
    //: 3 For concerns 3 & 4 perform the test using the exception-testing
    //:   infrastructure and verify the value and memory changes.
    //:
    //: 4 For concern 6, we select the value to insert from the middle of the
    //:   list, thus testing insertion before, at, and after the aliased
    //:   element.
    //:
    //: 5 For concern 7, save copies of the iterators before and after the
    //:   insertion point and verify that they point to the same (valid)
    //:   elements after the insertion by iterating to the same point in the
    //:   resulting list and comparing the new iterators to the old ones.
    //:
    //: 6 For concern 8, test each 'emplace' call with 0 to 5 arguments.  The
    //:   test types are designed to ignore all but the last argument, but
    //:   verify that the preceding arguments are the values '1', '2', '3', and
    //:   '4'.
    //
    // Testing:
    //   iterator insert(const_iterator pos, const T& value);
    //   iterator insert(const_iterator pos, size_type n, const T& value);
    //   void push_back(const T& value);
    //   void push_front(const T& value);
    //   iterator emplace(const_iterator pos, Args&&... args);
    //   reference emplace_back(Args&&... args);
    //   reference emplace_front(Args&&... args);
    //   template <class Iter> void insert(CIter pos, Iter f, Iter l);
    // -------------------------------------------------------------------

    TestValues VALUES("ABCDEFGH");

    bslma::TestAllocator oa("object",veryVeryVeryVerbose);
    const ALLOC          xoa(&oa);

    // Note that aix gives warnings for 'const TYPE& X = TYPE()' for any type
    // that has a private copy c'tor.  This will work for the set of 'TYPE's we
    // actually test.

    TYPE DEFAULT_VALUE;
    if (bsl::is_trivially_default_constructible<TYPE>::value) {
#if defined(BSLS_PLATFORM_CMP_MSVC)
// False warning about memory leak (constructor *is* trivial, placement new)
#pragma warning( push )
#pragma warning( disable : 4291 )
#endif
        new (bsls::Util::addressOf(DEFAULT_VALUE)) TYPE();
#if defined(BSLS_PLATFORM_CMP_MSVC)
// Turn back on locally disabled warnings
#pragma warning( pop )
#endif
    }

    const int           MAX_LEN    = 15;

    // Operations to test
    enum {
        TEST_FIRST,
        TEST_EMPLACE_A0 = TEST_FIRST, // emplace(pos);
        TEST_EMPLACE_A1,              // emplace(pos, a1);
        TEST_EMPLACE_A2,              // emplace(pos, a1, a2);
        TEST_EMPLACE_A3,              // emplace(pos, a1, a2, a3);
        TEST_EMPLACE_A4,              // emplace(pos, a1, a2, a3, a4);
        TEST_EMPLACE_A5,              // emplace(pos, a1, a2, a3, a4, a5);
        TEST_EMPLACE_FRONT_A0,        // emplace_front(pos);
        TEST_EMPLACE_FRONT_A1,        // emplace_front(pos, a1);
        TEST_EMPLACE_FRONT_A2,        // emplace_front(pos, a1, a2);
        TEST_EMPLACE_FRONT_A3,        // emplace_front(pos, a1, a2, a3);
        TEST_EMPLACE_FRONT_A4,        // emplace_front(pos, a1, a2, a3, a4);
        TEST_EMPLACE_FRONT_A5,        // emplace_front(pos, a1, a2, a3, a4,a5);
        TEST_EMPLACE_BACK_A0,         // emplace_back(pos);
        TEST_EMPLACE_BACK_A1,         // emplace_back(pos, a1);
        TEST_EMPLACE_BACK_A2,         // emplace_back(pos, a1, a2);
        TEST_EMPLACE_BACK_A3,         // emplace_back(pos, a1, a2, a3);
        TEST_EMPLACE_BACK_A4,         // emplace_back(pos, a1, a2, a3, a4);
        TEST_EMPLACE_BACK_A5,         // emplace_back(pos, a1, a2, a3, a4, a5);
        TEST_LAST
    };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // container spec
    } DATA[] = {
        //line  spec                            length
        //----  ----                            ------
        { L_,   ""                        }, // 0
        { L_,   "A"                       }, // 1
        { L_,   "AB"                      }, // 2
        { L_,   "ABC"                     }, // 3
        { L_,   "ABCD"                    }, // 4
        { L_,   "ABCDA"                   }, // 5
        { L_,   "ABCDABCDABCDABC"         }, // 15
    };
    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    // Iterate through the operations
    for (int op = TEST_FIRST; op < TEST_LAST; ++op) {

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int       LINE   = DATA[ti].d_lineNum;
            const char     *SPEC   = DATA[ti].d_spec_p;
            const unsigned  LENGTH = static_cast<unsigned>(std::strlen(SPEC));

            ASSERTV(LENGTH, LENGTH <= MAX_LEN);

            for (unsigned posidx = 0; posidx <= LENGTH; ++posidx) {

                if (TEST_EMPLACE_BACK_A0 <= op &&
                    op <= TEST_EMPLACE_BACK_A5 && LENGTH != posidx) {
                    continue;  // Can emplace_back only at end
                }
                else if (TEST_EMPLACE_FRONT_A0 <= op &&
                         op <= TEST_EMPLACE_FRONT_A5 && 0 != posidx) {
                    continue;  // Can emplace_front only at beginning
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const Int64 AL = oa.allocationLimit();
                    oa.setAllocationLimit(-1);

                    Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                    const Obj Y(X);  // Control

                    // Choose a value to insert that is deliberately aliasing a
                    // list element.

                    bool        useDefault = (TEST_EMPLACE_A0       == op ||
                                              TEST_EMPLACE_FRONT_A0 == op ||
                                              TEST_EMPLACE_BACK_A0  == op);
                    const TYPE& NEW_ELEM_REF(useDefault ?
                                             DEFAULT_VALUE :
                                             LENGTH ?
                                             nthElem(X,LENGTH/2) :
                                             VALUES[0]);
                    const TYPE  NEW_ELEM_VALUE(NEW_ELEM_REF);

                    // Save original iterators (including end iterator) C++0x
                    // allows insertion using const_iterator

                    const_iterator orig_iters[MAX_LEN + 1];
                    const_iterator it = X.begin();
                    for (unsigned i = 0; i < LENGTH + 1; ++i, ++it) {
                        orig_iters[i] = it;
                    }

                    oa.setAllocationLimit(AL);

                    // C++0x allows insertion using const_iterator
                    const_iterator    pos = orig_iters[posidx];
                    iterator          ret;
                    ExceptionProctor  proctor(&mX, LINE);
                    TYPE             *addressOfResult = 0;

                    const Int64 BB = oa.numBlocksTotal();
                    const Int64 B  = oa.numBlocksInUse();

                    switch (op) {
                        case TEST_EMPLACE_A0: {
                            ret = mX.emplace(pos);
                        } break;
                        case TEST_EMPLACE_A1: {
                            ret = mX.emplace(pos, NEW_ELEM_REF);
                        } break;
                        case TEST_EMPLACE_A2: {
                            ret = mX.emplace(pos, 1, NEW_ELEM_REF);
                        } break;
                        case TEST_EMPLACE_A3: {
                            ret = mX.emplace(pos, 1, 2, NEW_ELEM_REF);
                        } break;
                        case TEST_EMPLACE_A4: {
                            ret = mX.emplace(pos, 1, 2, 3, NEW_ELEM_REF);
                        } break;
                        case TEST_EMPLACE_A5: {
                            ret = mX.emplace(pos, 1, 2, 3, 4, NEW_ELEM_REF);
                        } break;
                        case TEST_EMPLACE_FRONT_A0: {
                            TYPE& result = mX.emplace_front();
                            addressOfResult = bsls::Util::addressOf(result);
                            ret = mX.begin();
                        } break;
                        case TEST_EMPLACE_FRONT_A1: {
                            TYPE& result = mX.emplace_front(NEW_ELEM_REF);
                            addressOfResult = bsls::Util::addressOf(result);
                            ret = mX.begin();
                        } break;
                        case TEST_EMPLACE_FRONT_A2: {
                            TYPE& result = mX.emplace_front(1,
                                                            NEW_ELEM_REF);
                            addressOfResult = bsls::Util::addressOf(result);
                            ret = mX.begin();
                        } break;
                        case TEST_EMPLACE_FRONT_A3: {
                            TYPE& result = mX.emplace_front(1,
                                                            2,
                                                            NEW_ELEM_REF);
                            addressOfResult = bsls::Util::addressOf(result);
                            ret = mX.begin();
                        } break;
                        case TEST_EMPLACE_FRONT_A4: {
                            TYPE& result = mX.emplace_front(1,
                                                            2,
                                                            3,
                                                            NEW_ELEM_REF);
                            addressOfResult = bsls::Util::addressOf(result);
                            ret = mX.begin();
                        } break;
                        case TEST_EMPLACE_FRONT_A5: {
                            TYPE& result = mX.emplace_front(1,
                                                            2,
                                                            3,
                                                            4,
                                                            NEW_ELEM_REF);
                            addressOfResult = bsls::Util::addressOf(result);
                            ret = mX.begin();
                        } break;
                        case TEST_EMPLACE_BACK_A0: {
                            TYPE& result = mX.emplace_back();
                            addressOfResult = bsls::Util::addressOf(result);
                            ret = --mX.end();
                        } break;
                        case TEST_EMPLACE_BACK_A1: {
                            TYPE& result = mX.emplace_back(NEW_ELEM_REF);
                            addressOfResult = bsls::Util::addressOf(result);
                            ret = --mX.end();
                        } break;
                        case TEST_EMPLACE_BACK_A2: {
                            TYPE& result = mX.emplace_back(1,
                                                           NEW_ELEM_REF);
                            addressOfResult = bsls::Util::addressOf(result);
                            ret = --mX.end();
                        } break;
                        case TEST_EMPLACE_BACK_A3: {
                            TYPE& result = mX.emplace_back(1,
                                                           2,
                                                           NEW_ELEM_REF);
                            addressOfResult = bsls::Util::addressOf(result);
                            ret = --mX.end();
                        } break;
                        case TEST_EMPLACE_BACK_A4: {
                            TYPE& result = mX.emplace_back(1,
                                                           2,
                                                           3,
                                                           NEW_ELEM_REF);
                            addressOfResult = bsls::Util::addressOf(result);
                            ret = --mX.end();
                        } break;
                        case TEST_EMPLACE_BACK_A5: {
                            TYPE& result = mX.emplace_back(1,
                                                           2,
                                                           3,
                                                           4,
                                                           NEW_ELEM_REF);
                            addressOfResult = bsls::Util::addressOf(result);
                            ret = --mX.end();
                        } break;
                        default: {
                            ASSERT(0 && "No such operation");
                        }
                    } // end switch
                    proctor.release();

                    // If got here, then there was no exception

                    const Int64 AA = oa.numBlocksTotal();
                    const Int64 A  = oa.numBlocksInUse();

                    // Test important values
                    ASSERTV(LINE, op, posidx, checkIntegrity(X, LENGTH + 1));
                    ASSERTV(LINE, op, posidx, LENGTH + 1 == X.size());
                    ASSERTV(LINE, op, posidx, BB + deltaBlocks(1) == AA);
                    ASSERTV(LINE, op, posidx, B + deltaBlocks(1) == A);

                    if (TEST_EMPLACE_A0 <= op && op <= TEST_EMPLACE_A5) {
                        // Test return value from emplace
                        ASSERTV(LINE, op, posidx,
                                     posDistance(mX.begin(), ret) == posidx);
                    }
                    if (TEST_EMPLACE_FRONT_A0 <= op
                     && op <= TEST_EMPLACE_FRONT_A5) {
                        // Test return value from emplace
                        const TYPE *ADDRESS_OF_FIRST_VALUE =
                                             bsls::Util::addressOf(X.front());
                        ASSERTV(LINE, op, posidx,
                                ADDRESS_OF_FIRST_VALUE == addressOfResult);
                    }

                    if (TEST_EMPLACE_BACK_A0 <= op
                     && op <= TEST_EMPLACE_BACK_A5) {
                        // Test return value from emplace
                        const TYPE *ADDRESS_OF_LAST_VALUE =
                                               bsls::Util::addressOf(X.back());
                        ASSERTV(LINE, op, posidx,
                                ADDRESS_OF_LAST_VALUE == addressOfResult);
                    }

                    const_iterator cit = X.begin();
                    const_iterator yi = Y.begin();
                    for (unsigned i = 0; i < X.size(); ++i, ++cit) {
                        if (i < posidx) {
                            // Test that part before insertion is unchanged
                            ASSERTV(LINE, op, posidx, i, *yi++ == *cit);
                            ASSERTV(LINE, op, posidx, i, orig_iters[i] == cit);
                        }
                        else if (i == posidx) {
                            // Test inserted value
                            ASSERTV(LINE, op, posidx, i,
                                         NEW_ELEM_VALUE == *cit);
                        }
                        else {
                            // Test that part after insertion is unchanged
                            ASSERTV(LINE, op, posidx, i, *yi++ == *cit);
                            ASSERTV(LINE, op, posidx, i,
                                         orig_iters[i - 1] == cit);
                        }
                    }
                    // Test end iterator
                    ASSERTV(LINE, op, posidx, X.end() == cit);
                    ASSERTV(LINE, op, posidx, orig_iters[LENGTH] == cit);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, op, posidx, 0 == oa.numBlocksInUse());
            } // end for (posidx)
        } // end for (ti)
    } // end for (op)
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test17_insert()
{
    // ------------------------------------------------------------------------
    // TESTING INSERT AND PUSH
    //
    // Concerns
    //: 1 That the resulting list value is correct.
    //:
    //: 2 That the 'insert' return (if any) value is a valid iterator to the
    //:   first inserted element or to the insertion position if no elements
    //:   are inserted.
    //:
    //: 3 That insertion of one element has the strong exception guarantee.
    //:
    //: 4 That insertion is exception neutral w.r.t. memory allocation.
    //:
    //: 5 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 6 That inserting a 'const T& value' that is a reference to an element
    //:   of the list does not suffer from aliasing problems.
    //:
    //: 7 That no iterators are invalidated by the insertion.
    //:
    //: 8 That inserting 'n' copies of value 'v' selects the correct overload
    //:   when 'n' and 'v' are identical arithmetic types (i.e., the
    //:   iterator-range overload is not selected).
    //:
    //: 9 That inserting 'n' copies of value 'v' selects the correct overload
    //:   when 'v' is a pointer type and 'n' is a null pointer literal ,'0'.
    //:   (i.e., the iterator-range overload is not selected).
    //
    // Plan:
    //: 1 Create objects of various sizes and insert a distinct value one or
    //:   more times into each possible position.
    //:   o For concerns 1, 2 & 5, verify that the return value and modified
    //:     list are as expected.
    //:   o For concerns 3 & 4 perform the test using the exception-testing
    //:     infrastructure and verify the value and memory changes.
    //:   o For concern 6, we select the value to insert from the middle of the
    //:     list, thus testing insertion before, at, and after the aliased
    //:     element.
    //:   o For concern 7, save copies of the iterators before and after the
    //:     insertion point and verify that they point to the same (valid)
    //:     elements after the insertion by iterating to the same point in the
    //:     resulting list and comparing the new iterators to the old ones.
    //:   o For concerns 8 and 9, insert 2 elements of integral or pointer
    //:     types into lists and verify that it compiles and that the resultant
    //:     list contains the expected values.
    //
    // Testing:
    //   iterator insert(const_iterator pos, const T& value);
    //   iterator insert(const_iterator pos, size_type n, const T& value);
    //   void push_back(const T& value);
    //   void push_front(const T& value);
    // -------------------------------------------------------------------

    TestValues VALUES("ABCDEFGH");

    const int  MAX_LEN    = 15;

    // Operations to test
    enum {
        TEST_FIRST,
        TEST_INSERT_N0 = TEST_FIRST,  // insert(pos, 0, value);
        TEST_INSERT_N1,               // insert(pos, 1, value);
        TEST_INSERT_N2,               // insert(pos, 2, value);
        TEST_INSERT_N3,               // insert(pos, 3, value);
        TEST_INSERT,                  // insert(pos, value);
        TEST_PUSH_BACK,               // push_back(value);
        TEST_PUSH_FRONT,              // push_front(value);
        TEST_LAST
    };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // container spec
    } DATA[] = {
        //line  spec                  length
        //----  -----------------     ------
        { L_,   ""                 }, // 0
        { L_,   "A"                }, // 1
        { L_,   "AB"               }, // 2
        { L_,   "ABC"              }, // 3
        { L_,   "ABCD"             }, // 4
        { L_,   "ABCDA"            }, // 5
        { L_,   "ABCDABCDABCDABC"  }, // 15
    };
    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    // Iterate through the operations
    for (int op = TEST_FIRST; op < TEST_LAST; ++op) {

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        const ALLOC          xoa(&oa);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int       LINE   = DATA[ti].d_lineNum;
            const char     *SPEC   = DATA[ti].d_spec_p;
            const unsigned  LENGTH = static_cast<int>(std::strlen(SPEC));

            ASSERTV(LENGTH, LENGTH <= MAX_LEN);

            for (unsigned posidx = 0; posidx <= LENGTH; ++posidx) {

                if (TEST_PUSH_BACK == op && LENGTH != posidx) {
                    continue;  // Can push_back only at end
                }
                else if (TEST_PUSH_FRONT == op && 0 != posidx) {
                    continue;  // Can push_front only at beginning
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const Int64 AL = oa.allocationLimit();
                    oa.setAllocationLimit(-1);

                    Obj        mX(xoa);
                    const Obj& X = gg(&mX, SPEC);

                    const Obj Y(X);  // Control

                    // Choose a value to insert that is deliberately aliasing a
                    // list element.

                    const TYPE& NEW_ELEM_REF(LENGTH ?
                                             nthElem(X,LENGTH/2) :
                                             VALUES[0]);
                    const TYPE  NEW_ELEM_VALUE(NEW_ELEM_REF);

                    // Save original iterators (including end iterator) C++0x
                    // allows insertion using const_iterator

                    const_iterator orig_iters[MAX_LEN + 1];
                    const_iterator it = X.begin();
                    for (unsigned i = 0; i < LENGTH + 1; ++i, ++it) {
                        orig_iters[i] = it;
                    }

                    oa.setAllocationLimit(AL);

                    unsigned         n = 0;
                    // C++0x allows insertion using const_iterator
                    const_iterator   pos = orig_iters[posidx];
                    iterator         ret;
                    ExceptionProctor proctor(&mX, LINE);

                    const Int64 B = oa.numBlocksInUse();

                    switch (op) {
                        case TEST_INSERT: {
                            ret = mX.insert(pos, NEW_ELEM_REF);
                            n = 1;
                        } break;
                        case TEST_PUSH_BACK: {
                            mX.push_back(NEW_ELEM_REF);
                            ret = --mX.end();
                            n = 1;
                        } break;
                        case TEST_PUSH_FRONT: {
                            mX.push_front(NEW_ELEM_REF);
                            ret = mX.begin();
                            n = 1;
                        } break;
                        default: {
                            n = op - TEST_INSERT_N0;
                            if (n > 1) {
                                // strong guarantee only for 0 or 1 insertion

                                proctor.release();
                            }
                            ret = mX.insert(pos, n, NEW_ELEM_REF);
                        }
                    } // end switch
                    proctor.release();

                    // If got here, then there was no exception

                    const Int64 A = oa.numBlocksInUse();

                    // Test important values
                    ASSERTV(LINE, op, posidx, checkIntegrity(X, LENGTH + n));
                    ASSERTV(LINE, op, posidx, LENGTH + n == X.size());
                    ASSERTV(LINE, op, posidx, B + deltaBlocks(n) == A);

                    // Test return value from 'insert'
                    ASSERTV(LINE, op, posidx,
                                 posDistance(mX.begin(), ret) == posidx);

                    const_iterator cit = X.begin();
                    const_iterator yi = Y.begin();
                    for (unsigned i = 0; i < X.size(); ++i, ++cit) {
                        if (i < posidx) {
                            // Test that part before insertion is unchanged
                            ASSERTV(LINE, op, posidx, i, *yi++ == *cit);
                            ASSERTV(LINE, op, posidx, i, orig_iters[i] == cit);
                        }
                        else if (i < posidx + n) {
                            // Test inserted values
                            ASSERTV(LINE, op, posidx, i,
                                                       NEW_ELEM_VALUE == *cit);
                        }
                        else {
                            // Test that part after insertion is unchanged
                            ASSERTV(LINE, op, posidx, i, *yi++ == *cit);
                            ASSERTV(LINE, op, posidx, i,
                                                     orig_iters[i - n] == cit);
                        }
                    }
                    // Test end iterator
                    ASSERTV(LINE, op, posidx, X.end() == cit);
                    ASSERTV(LINE, op, posidx, orig_iters[LENGTH] == cit);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, op, posidx, 0 == oa.numBlocksInUse());
            } // end for (posidx)
        } // end for (ti)
    } // end for (op)
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver2<TYPE,ALLOC>::test17_insertRange(const CONTAINER&)
{
    const int           MAX_LEN    = 15;

    // Starting data
    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // container spec
    } DATA[] = {
        //line  spec                  length
        //----  -----------------     ------
        { L_,   ""                 }, // 0
        { L_,   "A"                }, // 1
        { L_,   "AB"               }, // 2
        { L_,   "ABC"              }, // 3
        { L_,   "ABCD"             }, // 4
        { L_,   "ABCDA"            }, // 5
        { L_,   "ABCDABCDABCDABC"  }, // 15
    };
    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    // Data to insert
    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // container spec
    } U_DATA[] = {
        //line  spec      length
        //----  -----     ------
        { L_,   ""     }, // 0
        { L_,   "E"    }, // 1
        { L_,   "EA"   }, // 2
        { L_,   "EBA"  }, // 3
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int       LINE   = DATA[ti].d_lineNum;
        const char     *SPEC   = DATA[ti].d_spec_p;
        const unsigned  LENGTH = static_cast<unsigned>(std::strlen(SPEC));

        ASSERTV(LENGTH, LENGTH <= MAX_LEN);

        for (unsigned posidx = 0; posidx <= LENGTH; ++posidx) {

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            const ALLOC          xoa(&oa);

            for (unsigned ui = 0; ui < NUM_U_DATA; ++ui) {
                const int     U_LINE = U_DATA[ui].d_lineNum;
                const char   *U_SPEC = U_DATA[ui].d_spec_p;
                const size_t  N      = std::strlen(U_SPEC);

                CONTAINER        mU(U_SPEC);
                const CONTAINER& U = mU;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const Int64 AL = oa.allocationLimit();
                    oa.setAllocationLimit(-1);

                    Obj        mX(xoa);
                    const Obj& X = gg(&mX, SPEC);

                    const Obj Y(X);  // Control

                    // Save original iterators (including end iterator).  C++0x
                    // allows insertion using const_iterator

                    const_iterator orig_iters[MAX_LEN + 1];
                    const_iterator it = X.begin();
                    for (unsigned i = 0; i < LENGTH + 1; ++i, ++it) {
                        orig_iters[i] = it;
                    }

                    oa.setAllocationLimit(AL);

                    // C++0x allows insertion using const_iterator
                    const_iterator   pos = orig_iters[posidx];
                    iterator         ret;
                    ExceptionProctor proctor(&mX, LINE);

                    const Int64 B = oa.numBlocksInUse();

                    if (N > 1) {
                        // strong guarantee only for 0 or 1 insertions
                        proctor.release();
                    }

                    ret = mX.insert(pos, U.begin(), U.end());
                    proctor.release();

                    // If got here, then there was no exception

                    const Int64 A = oa.numBlocksInUse();

                    // Test important values
                    ASSERTV(LINE, posidx, U_LINE,
                                                checkIntegrity(X, LENGTH + N));
                    ASSERTV(LINE, posidx, U_LINE, LENGTH + N == X.size());
                    ASSERTV(LINE, posidx, U_LINE, B + deltaBlocks(N) == A);
                    ASSERTV(LINE, posidx, U_LINE,
                                       posDistance(mX.begin(), ret) == posidx);

                    const_iterator cit = X.begin();
                    const_iterator yi = Y.begin();
                    for (unsigned i = 0; i < X.size(); ++i, ++cit) {
                        if (i < posidx) {
                            // Test that part before insertion is unchanged
                            ASSERTV(LINE, posidx, U_LINE, i, *yi++ == *cit);
                            ASSERTV(LINE, posidx, U_LINE, i,
                                                         orig_iters[i] == cit);
                        }
                        else if (i < posidx + N) {
                            // Test inserted values
                            ASSERTV(LINE, posidx, U_LINE, i,
                                                        U[i - posidx] == *cit);
                        }
                        else {
                            // Test that part after insertion is unchanged
                            ASSERTV(LINE, posidx, U_LINE, i, *yi++ == *cit);
                            ASSERTV(LINE, posidx, U_LINE, i,
                                                     orig_iters[i - N] == cit);
                        }
                    }
                    // Test end iterator
                    ASSERTV(LINE, posidx, U_LINE, X.end() == cit);
                    ASSERTV(LINE, posidx, U_LINE, orig_iters[LENGTH] == cit);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, posidx, U_LINE, 0 == oa.numBlocksInUse());
            } // end for (ui)
        } // end for (posidx)
    } // end for (ti)
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test17_insertRange()
{
    // ------------------------------------------------------------------------
    // TESTING RANGE INSERTION:
    //
    // Concerns
    //: 1 That the resulting list value is correct.
    //:
    //: 2 That the 'insert' return (if any) value is a valid iterator to the
    //:   first inserted element or to the insertion position if no elements
    //:   are inserted.
    //:
    //: 3 That insertion of one element has the strong exception guarantee.
    //:
    //: 4 That insertion is exception neutral w.r.t. memory allocation.
    //:
    //: 5 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 6 That no iterators are invalidated by the insertion.
    //
    // Plan:
    //: 1 Create objects of various sizes and insert a range of 0 to 3 values
    //:   at each possible position.
    //:
    //: 2 For concerns 1, 2 & 5, verify that the return value and modified list
    //:   are as expected.
    //:
    //: 3 For concerns 3 & 4 perform the test using the exception-testing
    //:   infrastructure and verify the value and memory changes.
    //:
    //: 4 For concern 7, save copies of the iterators before and after the
    //:   insertion point and verify that they point to the same (valid)
    //:   elements after the insertion by iterating to the same point in the
    //:   resulting list and comparing the new iterators to the old ones.
    //
    // Testing:
    //   template <class Iter> void insert(CIter pos, Iter f, Iter l);
    // -------------------------------------------------------------------

    if (verbose) printf("... with an arbitrary forward iterator.\n");
    test17_insertRange(InputSeq<TYPE>());

    if (verbose) printf("... with an arbitrary random-access iterator.\n");
    test17_insertRange(RandSeq<TYPE>());
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test16_iterators()
{
    // ------------------------------------------------------------------------
    // TESTING ITERATORS
    //
    // Concerns:
    //: 1 That 'iterator' and 'const_iterator' are bi-directional iterators.
    //:
    //: 2 That 'iterator' and 'const_iterator' are CopyConstructible,
    //:   Assignable, and EqualityComparable, that 'iterator' is convertible to
    //:   'const_iterator', and that 'reverse_iterator' is constructible from
    //:   'iterator'.
    //:
    //: 3 That 'begin' and 'end' return mutable iterators for a reference to a
    //:   modifiable list, and non-mutable iterators otherwise.
    //:
    //: 4 That the iterators can be dereferenced using 'operator*' or
    //:   'operator->', yielding a reference or pointer with the correct
    //:   constness.
    //:
    //: 5 That the range '[begin(), end())' equals the value of the list.
    //:
    //: 6 That iterators can be pre-incremented, post-incremented,
    //:   pre-decremented, and post-decremented.
    //:
    //: 7 Same concerns with 'rbegin', 'rend', 'reverse_iterator', and
    //:   'const_reverse_iterator'.
    //
    // Plan:
    //: 1 For concerns 1, 3, 4, and 7 create a one-element list and verify the
    //:   static properties of 'iterator', 'const_iterator',
    //:   ''reverse_iterator', and 'const_reverse_iterator'.
    //:
    //: 2 For concerns 1, 2, 5, 6, and 7, for each value given by variety of
    //:   specifications of different lengths, create a test list with this
    //:   value, and access each element in sequence and in reverse sequence,
    //:   both as a reference providing modifiable access (setting it to a
    //:   default value, then back to its original value), and as a reference
    //:   providing non-modifiable access.  At each step in the traversal, save
    //:   the current iterator using both copy construction and assignment and,
    //:   in a nested second loop, traverse the whole list in reverse order,
    //:   testing that the nested-traversal iterator matches the saved iterator
    //:   if and only if they refer to the same element.
    //
    // Testing:
    //   type iterator
    //   type reverse_iterator
    //   type const_iterator
    //   type const_reverse_iterator
    //   iterator begin();
    //   iterator end();
    //   reverse_iterator rbegin();
    //   reverse_iterator rend();
    //   const_iterator begin() const;
    //   const_iterator end() const;
    //   const_reverse_iterator rbegin() const;
    //   const_reverse_iterator rend() const;
    // ------------------------------------------------------------------------

    typedef typename Obj::size_type size_type;

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec_p;           // initial
    } DATA[] = {
        //line  spec
        //----  -----------
        { L_,   ""           },
        { L_,   "A"          },
        { L_,   "ABC"        },
        { L_,   "ABCD"       },
        { L_,   "ABCDE"      },
        { L_,   "ABCDEAB"    },
        { L_,   "ABCDEABC"   },
        { L_,   "ABCDEABCD"  }
    };
    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    if (verbose) printf("test16_iterators<%s>: Testing\n",
                        bsls::NameOf<TYPE>().name());
    {
        Obj        mX(2);
        const Obj& X = mX;

        const iterator               iter   = mX.begin();
        const const_iterator         citer  =  X.begin();
        const reverse_iterator       riter  = mX.rbegin();
        const const_reverse_iterator criter =  X.rbegin();

        // Check iterator category
        ASSERT((bsl::is_same<typename iterator::iterator_category,
                             std::bidirectional_iterator_tag>::value));
        ASSERT((bsl::is_same<typename reverse_iterator::iterator_category,
                             std::bidirectional_iterator_tag>::value));
        ASSERT((bsl::is_same<typename const_iterator::iterator_category,
                             std::bidirectional_iterator_tag>::value));
        ASSERT((bsl::is_same<typename const_reverse_iterator::iterator_category
                            ,std::bidirectional_iterator_tag>::value));

        // Test mutability
        ASSERT(  is_mutable(*mX.begin()));
        ASSERT(! is_mutable(* X.begin()));
        ASSERT(  is_mutable(*mX.rbegin()));
        ASSERT(! is_mutable(* X.rbegin()));
        ASSERT(  is_mutable(*--mX.end()));
        ASSERT(! is_mutable(*-- X.end()));
        ASSERT(  is_mutable(*--mX.rend()));
        ASSERT(! is_mutable(*-- X.rend()));

        ASSERT(  is_mutable(*iter));
        ASSERT(! is_mutable(*citer));
        ASSERT(  is_mutable(*riter));
        ASSERT(! is_mutable(*criter));

        // Test dereferencing
        ASSERT(bsls::Util::addressOf(*iter)   ==
                                             bsls::Util::addressOf(X.front()));
        ASSERT(bsls::Util::addressOf(*citer)  ==
                                             bsls::Util::addressOf(X.front()));
        ASSERT(bsls::Util::addressOf(*riter)  ==
                                             bsls::Util::addressOf(X.back()));
        ASSERT(bsls::Util::addressOf(*criter) ==
                                             bsls::Util::addressOf(X.back()));

        // Test operator->()
        ASSERT(iter.operator->()   == bsls::Util::addressOf(X.front()));
        ASSERT(citer.operator->()  == bsls::Util::addressOf(X.front()));
        ASSERT(riter.operator->()  == bsls::Util::addressOf(X.back()));
        ASSERT(criter.operator->() == bsls::Util::addressOf(X.back()));
    }

    if (veryVerbose) printf("Testing 'begin', and 'end', 'rbegin', 'rend', "
                            " and their 'const' variants.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        ALLOC                xoa(&oa);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = std::strlen(SPEC);

            Obj        mX(xoa);
            const Obj& X = gg(&mX, SPEC);

            Obj        mY;
            const Obj& Y = gg(&mY, SPEC);  // control

            if (veryVerbose) { P_(LINE); P(SPEC); }

            iterator               iter   = mX.begin();
            const_iterator         citer  =  X.begin();
            reverse_iterator       riter  = mX.rend();
            const_reverse_iterator criter =  X.rend();

            for (size_type i = 0; i < LENGTH; ++i)
            {
                TYPE        viRaw;
                const TYPE& vi = viRaw;

                assignTo(bsls::Util::addressOf(viRaw), SPEC[i]);

                ASSERTV(LINE, i, vi == *iter);
                ASSERTV(LINE, i, vi == *citer);

                ASSERTV(LINE, i, iter   != mX.end());
                ASSERTV(LINE, i, citer  != X.end());
                ASSERTV(LINE, i, riter  != mX.rbegin());
                ASSERTV(LINE, i, criter != X.rbegin());
                ASSERTV(LINE, i, citer  == iter);
                ASSERTV(LINE, i, bsls::Util::addressOf(*citer) ==
                                                 bsls::Util::addressOf(*iter));

                // Decrement reverse iterator before dereferencing

                --riter;
                --criter;

                // Reverse iterator refers to same element as iterator

                ASSERTV(LINE, i, vi == *riter);
                ASSERTV(LINE, i, vi == *criter);
                ASSERTV(LINE, i, bsls::Util::addressOf(*iter) ==
                                                bsls::Util::addressOf(*riter));
                ASSERTV(LINE, i, bsls::Util::addressOf(*iter) ==
                                               bsls::Util::addressOf(*criter));

                // iterator copy ctor and assignment

                iterator iter2(iter);     // iterator copy ctor
                iterator iter3(mY.end());
                iter3 = iter;             // iterator assignment

                // const_iterator copy ctor, assignment, and conversion

                const_iterator citer2(citer);   // const_iterator copy ctor
                const_iterator citer3(Y.end());
                citer3 = citer;                 // const_iterator assignment
                const_iterator citer4 = iter;   // conversion ctor
                const_iterator citer5(Y.end());
                citer5 = iter;                  // conversion assignment

                // reverse_iterator conversion, copy ctor, and assignment

                reverse_iterator       riter1(iter);     // conversion ctor
                reverse_iterator       riter2(riter);    // copy ctor
                reverse_iterator       riter3(mY.end());
                riter3 = riter;                          // assignment

                // const_reverse_iterator conversion, copy ctor, and assignment

                const_reverse_iterator criter1(citer);   // rev conversion ctor
                const_reverse_iterator criter3(criter);  // copy ctor
                const_reverse_iterator criter2(riter);   // const conversion
                const_reverse_iterator criter4(Y.end());
                criter4 = criter;                        // assignment
                const_reverse_iterator criter5(Y.end());
                criter5 = riter;                         // const assignment

                // Test equivalences: All of the iterators except for riter1
                // and criter1 refer to the same element

                ASSERTV(LINE, i, iter   == iter2);
                ASSERTV(LINE, i, iter   == iter3);
                ASSERTV(LINE, i, citer  == citer2);
                ASSERTV(LINE, i, citer  == citer3);
                ASSERTV(LINE, i, citer  == citer4);
                ASSERTV(LINE, i, citer  == citer5);
                ASSERTV(LINE, i, riter  == riter2);
                ASSERTV(LINE, i, riter  == riter3);

                ASSERTV(LINE, i, criter == criter2);
                ASSERTV(LINE, i, criter == criter3);
                ASSERTV(LINE, i, criter == criter4);
                ASSERTV(LINE, i, criter == criter5);

                ASSERTV(LINE, i, bsls::Util::addressOf(*iter)   ==
                                                bsls::Util::addressOf(*iter2));
                ASSERTV(LINE, i, bsls::Util::addressOf(*citer)  ==
                                               bsls::Util::addressOf(*citer2));
                ASSERTV(LINE, i, bsls::Util::addressOf(*riter)  ==
                                               bsls::Util::addressOf(*riter2));
                ASSERTV(LINE, i, bsls::Util::addressOf(*criter) ==
                                              bsls::Util::addressOf(*criter2));

                // Forward-reverse equivalences

                ASSERTV(LINE, i, citer == criter1.base());
                ASSERTV(LINE, i, iter  == riter1.base());
                ASSERTV(LINE, i, bsls::Util::addressOf(*iter)  ==
                                             bsls::Util::addressOf(*--riter1));
                ASSERTV(LINE, i, bsls::Util::addressOf(*citer) ==
                                            bsls::Util::addressOf(*--criter1));

                // Iterate backwards over the list

                iterator               iback   = mX.end();
                const_iterator         ciback  =  X.end();
                reverse_iterator       riback  = mX.rbegin();
                const_reverse_iterator criback =  X.rbegin();
                for (size_type j = LENGTH; j > 0; ) {
                    --j;
                    --iback;
                    --ciback;

                    ASSERTV(LINE, i, j, bsls::Util::addressOf(*iback) ==
                                               bsls::Util::addressOf(*ciback));
                    ASSERTV(LINE, i, j, bsls::Util::addressOf(*iback) ==
                                               bsls::Util::addressOf(*riback));
                    ASSERTV(LINE, i, j, bsls::Util::addressOf(*iback) ==
                                              bsls::Util::addressOf(*criback));

                    const bool is_eq  = (j == i);
                    const bool is_neq = (j != i);

                    ASSERTV(LINE, i, j, is_eq  == (iback   == iter2));
                    ASSERTV(LINE, i, j, is_neq == (iback   != iter2));
                    ASSERTV(LINE, i, j, is_eq  == (ciback  == citer2));
                    ASSERTV(LINE, i, j, is_neq == (ciback  != citer2));
                    ASSERTV(LINE, i, j, is_eq  == (riback  == riter2));
                    ASSERTV(LINE, i, j, is_neq == (riback  != riter2));
                    ASSERTV(LINE, i, j, is_eq  == (criback == criter2));
                    ASSERTV(LINE, i, j, is_neq == (criback != criter2));

                    ASSERTV(LINE, i, j,
                            is_eq == (bsls::Util::addressOf(*iback) ==
                                               bsls::Util::addressOf(*iter2)));
                    ASSERTV(LINE, i, j,
                            is_eq == (bsls::Util::addressOf(*iback) ==
                                              bsls::Util::addressOf(*citer2)));
                    ASSERTV(LINE, i, j,
                            is_eq == (bsls::Util::addressOf(*iback) ==
                                              bsls::Util::addressOf(*riter2)));
                    ASSERTV(LINE, i, j,
                            is_eq == (bsls::Util::addressOf(*iback) ==
                                             bsls::Util::addressOf(*criter2)));

                    ++riback;
                    ++criback;
                    // 'iback' and 'ciback' have already been decremented
                }

                ASSERTV(LINE, i, X.begin() == iback);
                ASSERTV(LINE, i, X.begin() == ciback);

                // C++0x allows comparison of dissimilar reverse_iterators.
                //ASSERTV(LINE, i, X.rend()  == riback);

                ASSERTV(LINE, i, X.rend()  == criback);

                // Test result of pre and post-increment

                ASSERTV(LINE, i, iter2++   ==   iter3);
                ASSERTV(LINE, i, iter2     == ++iter3);
                ASSERTV(LINE, i, iter2     ==   iter3);
                ASSERTV(LINE, i, citer2++  ==   citer3);
                ASSERTV(LINE, i, citer2    == ++citer3);
                ASSERTV(LINE, i, citer2    ==   citer3);
                ASSERTV(LINE, i, riter2++  ==   riter3);
                ASSERTV(LINE, i, riter2    == ++riter3);
                ASSERTV(LINE, i, riter2    ==   riter3);
                ASSERTV(LINE, i, criter2++ ==   criter3);
                ASSERTV(LINE, i, criter2   == ++criter3);
                ASSERTV(LINE, i, criter2   ==   criter3);

                ++iter;
                ++citer;
                // 'riter' and 'criter' have already been decremented
            } // end for i

            ASSERTV(LINE, X.end() == iter);
            ASSERTV(LINE, X.end() == citer);

            // C++0x allows comparison of dissimilar reverse_iterators.
            //ASSERTV(LINE, X.rbegin() == riter);

            ASSERTV(LINE, X.rbegin() == criter);
        } // end for each spec
    } // end for 'begin', 'end', etc.
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test15_elementAccess()
{
    // ------------------------------------------------------------------------
    // TESTING ELEMENT ACCESS
    //
    // Concerns:
    //: 1 That 'v.front()' and 'v.back()', allow modifying the element when 'v'
    //:   is modifiable, but must not modify the element when it is 'const'.
    //
    // Plan:
    //: 1 For each value given by variety of specifications of different
    //:   lengths, create a test list with this value, and access the first and
    //:   last elements (front, back) both as a reference providing modifiable
    //:   access (setting it to a default value, then back to its original
    //:   value), and as a reference providing non-modifiable access.
    //
    // Testing:
    //   T& front();
    //   T& back();
    //   const T& front() const;
    //   const T& back() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("testElementAccess for TYPE: %s\n",
                        bsls::NameOf<TYPE>().name());

    TestValues VALUES("ABCDEFGH");
    const int  NUM_VALUES = 8;

    const TYPE& STRANGE_VALUE = VALUES[NUM_VALUES - 1];

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec_p;           // initial
    } DATA[] = {
        //line  spec
        //----  -----------
        { L_,   ""           },
        { L_,   "A"          },
        { L_,   "ABC"        },
        { L_,   "ABCD"       },
        { L_,   "ABCDE"      },
        { L_,   "ABCDEAB"    },
        { L_,   "ABCDEABC"   },
        { L_,   "ABCDEABCD"  }
    };
    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = std::strlen(SPEC);

            if (veryVerbose) { T_; P(SPEC); }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            ALLOC                xoa(&oa);

            Obj        mX(xoa);
            const Obj& X = gg(&mX,SPEC);

            Obj        mY;
            const Obj& Y = gg(&mY, SPEC); // control

            if (verbose) { P_(LINE); P(SPEC); }

            if (LENGTH) {
                TYPE expFront;    const TYPE& EXP_FRONT = expFront;
                assignTo(bsls::Util::addressOf(expFront), SPEC[0]);

                TYPE expBack;     const TYPE& EXP_BACK  = expBack;
                assignTo(bsls::Util::addressOf(expBack), SPEC[LENGTH - 1]);

                ASSERTV(LINE,   is_mutable(mX.front()));
                ASSERTV(LINE, ! is_mutable(X.front()));
                ASSERTV(LINE, bsls::NameOf<TYPE>(), value_of(EXP_FRONT),
                                  value_of(X.front()), EXP_FRONT == X.front());

                assignTo<TYPE>(bsls::Util::addressOf(mX.front()),
                               value_of(STRANGE_VALUE));

                ASSERTV(LINE, STRANGE_VALUE == X.front());
                ASSERTV(LINE, Y != X);

                assignTo<TYPE>(bsls::Util::addressOf(mX.front()),
                               value_of(Y.front()));

                ASSERTV(LINE, Y == X);

                ASSERTV(LINE,   is_mutable(mX.back()));
                ASSERTV(LINE, ! is_mutable(X.back()));
                ASSERTV(LINE, bsls::NameOf<TYPE>(), value_of(EXP_BACK),
                                     value_of(X.back()), EXP_BACK == X.back());

                assignTo<TYPE>(bsls::Util::addressOf(mX.back()),
                              value_of(STRANGE_VALUE));

                ASSERTV(LINE, STRANGE_VALUE == X.back());
                ASSERTV(LINE, Y != X);
                assignTo<TYPE>(bsls::Util::addressOf(mX.back()),
                               value_of(Y.back()));
                ASSERTV(LINE, Y == X);
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test14_resizeDefault()
{
    // ------------------------------------------------------------------------
    // TESTING 'resize'
    //
    // Concerns:
    //:  1 Resized list has the correct value.
    //:
    //:  2 Resizing to the current size allocates and frees no memory.
    //:
    //:  3 Resizing to a smaller size allocates no memory.
    //:
    //:  4 Resizing to a larger size frees no memory.
    //:
    //:  5 Resizing to a larger size propagates the allocator to elements
    //:    appropriately.
    //:
    //:  6 'resize' is exception neutral.
    //
    // Plan:
    //: 1 Using a set of input specs and result sizes, try each combination
    //:   with and without specifying a value for the new elements.  Verify
    //:   each of the above concerns for each combination.
    //
    // Testing:
    //   void resize(size_type n);
    //   void resize(size_type n, const T& val);
    // ------------------------------------------------------------------------

    // Note that aix gives warnings for 'const TYPE& X = TYPE()' for any type
    // that has a private copy c'tor.  This will work for the set of 'TYPE's we
    // actually test.

    TYPE DEFAULT_VALUE;
    if (bsl::is_trivially_default_constructible<TYPE>::value) {
#if defined(BSLS_PLATFORM_CMP_MSVC)
// False warning about memory leak (constructor *is* trivial, placement new)
#pragma warning( push )
#pragma warning( disable : 4291 )
#endif
        new (bsls::Util::addressOf(DEFAULT_VALUE)) TYPE();
#if defined(BSLS_PLATFORM_CMP_MSVC)
// Turn back on locally disabled warnings
#pragma warning( pop )
#endif
    }

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // container spec
    } DATA[] = {
        //line  spec                    length
        //----  -------------------     ------
        { L_,   ""                   }, // 0
        { L_,   "A"                  }, // 1
        { L_,   "AB"                 }, // 2
        { L_,   "ABC"                }, // 3
        { L_,   "ABCD"               }, // 4
        { L_,   "ABCDE"              }, // 5
        { L_,   "ABCDEAB"            }, // 7
        { L_,   "ABCDEABC"           }, // 8
        { L_,   "ABCDEABCD"          }, // 9
        { L_,   "ABCDEABCDEABCDE"    }, // 15
        { L_,   "ABCDEABCDEABCDEA"   }, // 16
        { L_,   "ABCDEABCDEABCDEAB"  }  // 17
    };
    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    if (verbose) printf("Testing resize(sz).\n");
    for (int i = 0; i < NUM_DATA; ++i)
    {
        const int     LINE   = DATA[i].d_lineNum;
        const char   *SPEC   = DATA[i].d_spec_p;
        const size_t  LENGTH = std::strlen(SPEC);

        if (veryVerbose) { T_; P(SPEC); }

        bslma::TestAllocator  oa("object", veryVeryVeryVerbose);
        ALLOC                 xoa(&oa);

        for (size_t newlen = 0; newlen < 20; ++newlen)
        {
            const size_t NEWLEN = newlen;
            if (veryVerbose) { T_; T_; P(NEWLEN); }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                const Int64 AL = oa.allocationLimit();
                oa.setAllocationLimit(-1);

                Obj        mX(xoa);
                const Obj& X = gg(&mX, SPEC);

                Obj        mU(xoa);
                const Obj& U = gg(&mU, SPEC);

                oa.setAllocationLimit(AL);

                const Int64 BB = oa.numBlocksTotal();
                const Int64 B  = oa.numBlocksInUse();

                mX.resize(NEWLEN);  // test here

                const Int64 AA = oa.numBlocksTotal();
                const Int64 A  = oa.numBlocksInUse();

                if (veryVerbose) { T_; T_; T_; P(X); }

                ASSERTV(LINE, NEWLEN, checkIntegrity(X, NEWLEN));
                ASSERTV(LINE, NEWLEN, NEWLEN == X.size());
                if (NEWLEN <= LENGTH) {
                    ASSERTV(LINE, NEWLEN, BB == AA);
                }
                else {
                    ASSERTV(LINE, NEWLEN,
                                      BB + deltaBlocks(NEWLEN - LENGTH) == AA);
                }

                ptrdiff_t difference = static_cast<ptrdiff_t>(NEWLEN - LENGTH);
                ASSERTV(LINE, NEWLEN, B + deltaBlocks(difference) == A);

                const_iterator xi = X.begin();
                const_iterator yi = U.begin();
                for (size_t j = 0;
                     j <  LENGTH && j < NEWLEN;
                     ++j, ++xi, ++yi) {
                    ASSERTV(LINE, NEWLEN, *yi == *xi);
                }

                for (size_t j = LENGTH; j < NEWLEN; ++j, ++xi) {
                    if (k_IS_DEFAULT_CONSTRUCTIBLE) {
                        ASSERTV(LINE, NEWLEN, DEFAULT_VALUE == *xi);
                    }
                }
                ASSERTV(LINE, NEWLEN, xi == X.end());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == oa.numMismatches());
            ASSERT(0 == oa.numBlocksInUse());
        } // end for newlen
    } // end testing resize(sz)
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test14_resizeNonDefault()
{
    TestValues VALUES("ABCDEFGH");
    const int  NUM_VALUES = 8;

    // Note that aix gives warnings for 'const TYPE& X = TYPE()' for any type
    // that has a private copy c'tor.  This will work for the set of 'TYPE's we
    // actually test.

    TYPE DEFAULT_VALUE;
    if (bsl::is_trivially_default_constructible<TYPE>::value) {
#if defined(BSLS_PLATFORM_CMP_MSVC)
// False warning about memory leak (constructor *is* trivial, placement new)
#pragma warning( push )
#pragma warning( disable : 4291 )
#endif
        new (bsls::Util::addressOf(DEFAULT_VALUE)) TYPE();
#if defined(BSLS_PLATFORM_CMP_MSVC)
// Turn back on locally disabled warnings
#pragma warning( pop )
#endif
    }

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // container spec
    } DATA[] = {
        //line  spec                    length
        //----  -------------------     ------
        { L_,   ""                   }, // 0
        { L_,   "A"                  }, // 1
        { L_,   "AB"                 }, // 2
        { L_,   "ABC"                }, // 3
        { L_,   "ABCD"               }, // 4
        { L_,   "ABCDE"              }, // 5
        { L_,   "ABCDEAB"            }, // 7
        { L_,   "ABCDEABC"           }, // 8
        { L_,   "ABCDEABCD"          }, // 9
        { L_,   "ABCDEABCDEABCDE"    }, // 15
        { L_,   "ABCDEABCDEABCDEA"   }, // 16
        { L_,   "ABCDEABCDEABCDEAB"  }  // 17
    };
    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    if (verbose) printf("Testing resize(sz).\n");
    for (int i = 0; i < NUM_DATA; ++i)
    {
        const int     LINE   = DATA[i].d_lineNum;
        const char   *SPEC   = DATA[i].d_spec_p;
        const size_t  LENGTH = std::strlen(SPEC);

        if (veryVerbose) { T_; P(SPEC); }

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        ALLOC                xoa(&oa);

        for (size_t newlen = 0; newlen < 20; ++newlen)
        {
            const size_t NEWLEN = newlen;
            if (veryVerbose) { T_; T_; P(NEWLEN); }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                const Int64 AL = oa.allocationLimit();
                oa.setAllocationLimit(-1);

                Obj        mX(xoa);
                const Obj& X = gg(&mX, SPEC);

                Obj        mU(X);
                const Obj& U = mU;

                oa.setAllocationLimit(AL);

                const Int64 BB = oa.numBlocksTotal();
                const Int64 B  = oa.numBlocksInUse();

                mX.resize(NEWLEN);  // test here

                const Int64 AA = oa.numBlocksTotal();
                const Int64 A  = oa.numBlocksInUse();

                if (veryVerbose) { T_; T_; T_; P(X); }

                ASSERTV(LINE, NEWLEN, checkIntegrity(X, NEWLEN));
                ASSERTV(LINE, NEWLEN, NEWLEN == X.size());
                if (NEWLEN <= LENGTH) {
                    ASSERTV(LINE, NEWLEN, BB == AA);
                }
                else {
                    ASSERTV(LINE, NEWLEN,
                                 BB + deltaBlocks(NEWLEN - LENGTH) == AA);
                }

                ptrdiff_t difference = static_cast<ptrdiff_t>(NEWLEN - LENGTH);
                ASSERTV(LINE, NEWLEN, B + deltaBlocks(difference) == A);

                const_iterator xi = X.begin();
                const_iterator yi = U.begin();

                for (size_t j = 0;
                     j <  LENGTH && j < NEWLEN;
                     ++j, ++xi, ++yi) {
                    ASSERTV(LINE, NEWLEN, *yi == *xi);
                }

                for (size_t j = LENGTH; j < NEWLEN; ++j, ++xi) {
                    if (k_IS_DEFAULT_CONSTRUCTIBLE) {
                        ASSERTV(LINE, NEWLEN, DEFAULT_VALUE == *xi);
                    }
                }
                ASSERTV(LINE, NEWLEN, xi == X.end());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == oa.numMismatches());
            ASSERT(0 == oa.numBlocksInUse());
        } // end for newlen
    } // end testing resize(sz)

    if (verbose) printf("Testing resize(sz, c).\n");
    for (int i = 0; i < NUM_DATA; ++i)
    {
        const int     LINE   = DATA[i].d_lineNum;
        const char   *SPEC   = DATA[i].d_spec_p;
        const size_t  LENGTH = std::strlen(SPEC);
        const TYPE    VALUE  = VALUES[i % NUM_VALUES];

        if (veryVerbose) { T_; P(SPEC); }

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        ALLOC                xoa(&oa);

        for (size_t newlen = 0; newlen < 20; ++newlen)
        {
            const size_t NEWLEN = newlen;
            if (veryVerbose) { T_; T_; P(NEWLEN); }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                const Int64 AL = oa.allocationLimit();
                oa.setAllocationLimit(-1);

                Obj        mX(xoa);
                const Obj& X = gg(&mX, SPEC);

                Obj        mU(X);
                const Obj& U = mU;

                oa.setAllocationLimit(AL);

                const Int64 BB = oa.numBlocksTotal();
                const Int64 B  = oa.numBlocksInUse();

                mX.resize(NEWLEN, VALUE);  // test here

                const Int64 AA = oa.numBlocksTotal();
                const Int64 A  = oa.numBlocksInUse();

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                ASSERTV(LINE, NEWLEN, checkIntegrity(X, NEWLEN));
                ASSERTV(LINE, NEWLEN, NEWLEN == X.size());
                if (NEWLEN <=  LENGTH) {
                    ASSERTV(LINE, NEWLEN, BB == AA);
                }
                else {
                    ASSERTV(LINE, NEWLEN,
                                 BB + deltaBlocks(NEWLEN - LENGTH) == AA);
                }

                ptrdiff_t difference = static_cast<ptrdiff_t>(NEWLEN - LENGTH);
                ASSERTV(LINE, NEWLEN, B + deltaBlocks(difference) == A);

                const_iterator xi = X.begin();
                const_iterator yi = U.begin();
                for (size_t j = 0;
                     j < LENGTH && j < NEWLEN;
                     ++j, ++xi, ++yi) {
                    ASSERTV(LINE, NEWLEN, *yi == *xi);
                }
                for (size_t j = LENGTH; j < NEWLEN; ++j, ++xi) {
                    ASSERTV(LINE, NEWLEN, VALUE == *xi);
                }
                ASSERTV(LINE, NEWLEN, xi == X.end());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == oa.numMismatches());
            ASSERT(0 == oa.numBlocksInUse());
        } // end for newlen
    } // end testing resize(sz)
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test14_resize(bsl::true_type hasNoCopyCtor)
{
    (void) hasNoCopyCtor;

    test14_resizeDefault();
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test14_resize(bsl::false_type hasNoCopyCtor)
{
    (void) hasNoCopyCtor;

    test14_resizeDefault();
    test14_resizeNonDefault();
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test14_resize()
{
    // ------------------------------------------------------------------------
    // TESTING RESIZE
    //
    // Concerns:
    //: 1 Resized list has the correct value.
    //:
    //: 2 Resizing to the current size allocates and frees no memory.
    //:
    //: 3 Resizing to a smaller size allocates no memory.
    //:
    //: 4 Resizing to a larger size frees no memory.
    //:
    //: 5 Resizing to a larger size propagates the allocator to elements
    //:   appropriately.
    //:
    //: 6 'resize' is exception neutral.
    //
    // Plan:
    //: 1 Using a set of input specs and result sizes, try each combination
    //:   with and without specifying a value for the new elements.  Verify
    //:   each of the above concerns for each combination.
    //
    // Testing:
    //   void resize(size_type n);
    //   void resize(size_type n, const T& val);
    // ------------------------------------------------------------------------

    typedef typename bsl::integral_constant<bool,
                     bsl::is_same<bsltf::MoveOnlyAllocTestType, TYPE>::value ||
                     bsl::is_same<bsltf::WellBehavedMoveOnlyAllocTestType,
                                             TYPE>::value>::type HasNoCopyCtor;
    test14_resize(HasNoCopyCtor());
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test13_initialLengthAssign()
{
    // ------------------------------------------------------------------------
    // TESTING INITIAL-LENGTH AND RANGE ASSIGNMENT
    //
    // Concerns:
    //: 1 The assigned value is correct.
    //:
    //: 2 The 'assign' call is exception neutral w.r.t. memory allocation.
    //:
    //: 3 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //
    // Plan:
    //: 1 For the assignment we will create objects of varying sizes containing
    //:   default values for type T, and then assign different 'value'.
    //:   Perform the above tests:
    //:   o With various initial values before the assignment.
    //:   o In the presence of exceptions during memory allocations using a
    //:     'bslma_TestAllocator' and varying its *allocation* *limit*.
    //:
    //: 2 Use basic accessors and equality comparison to verify that assignment
    //:   was successful.
    //
    // Note that we relax the concerns about memory consumption, since this is
    // implemented as 'erase + insert', and insert will be tested more
    // completely in test case 17.
    //
    // Testing:
    //   assign(size_type n, const T& value);
    //   template <class Iter> assign(Iter first, Iter last);
    // ------------------------------------------------------------------------
    TestValues VALUES("ABCDEFGH");
    const int  NUM_VALUES = 8;

    // Note that aix gives warnings for 'const TYPE& X = TYPE()' for any type
    // that has a private copy c'tor.  This will work for the set of 'TYPE's we
    // actually test.

    TYPE DEFAULT_VALUE;
    if (bsl::is_trivially_default_constructible<TYPE>::value) {
#if defined(BSLS_PLATFORM_CMP_MSVC)
// False warning about memory leak (constructor *is* trivial, placement new)
#pragma warning( push )
#pragma warning( disable : 4291 )
#endif
        new (bsls::Util::addressOf(DEFAULT_VALUE)) TYPE();
#if defined(BSLS_PLATFORM_CMP_MSVC)
// Turn back on locally disabled warnings
#pragma warning( pop )
#endif
    }

    if (verbose) printf("Testing initial-length assignment.\n");
    {
        static const struct {
            int d_lineNum;  // source line number
            int d_length;   // expected length
        } DATA[] = {
            //line  length
            //----  ------
            { L_,        0  },
            { L_,        1  },
            { L_,        2  },
            { L_,        3  },
            { L_,        4  },
            { L_,        5  },
            { L_,        6  },
            { L_,        7  },
            { L_,        8  },
            { L_,        9  },
            { L_,       11  },
            { L_,       12  },
            { L_,       14  },
            { L_,       15  },
            { L_,       16  },
            { L_,       17  }
        };
        enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };


        if (verbose) printf("\tUsing 'n' copies of 'value'.\n");
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            ALLOC                xoa(&oa);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                    printf(" using default value.\n");
                }

                Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xoa);
                const Obj& X = mX;

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    mX.assign(LENGTH, VALUE);
                    const Int64 A = oa.numBlocksInUse();

                    if (veryVerbose) {
                        T_; T_; T_; P(X);
                    }

                    ASSERTV(INIT_LINE, LINE, i, ti, checkIntegrity(X, LENGTH));
                    ASSERTV(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                    ASSERTV(INIT_LINE, LINE, i, ti,
                                                  A == expectedBlocks(LENGTH));

                    for (const_iterator j = X.begin(); j != X.end(); ++j) {
                        ASSERTV(INIT_LINE,LINE, i, ti, VALUE == *j);
                    }
                }
            }
            ASSERT(0 == oa.numMismatches());
            ASSERT(0 == oa.numBlocksInUse());
        }

        if (verbose) printf("\tWith exceptions.\n");
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            ALLOC                xoa(&oa);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                    printf(" using default value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        const Int64 AL = oa.allocationLimit();
                        oa.setAllocationLimit(-1);

                        Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xoa);
                        const Obj& X = mX;

                        oa.setAllocationLimit(AL);

                        mX.assign(LENGTH, VALUE);  // test here
                        const Int64 A = oa.numBlocksInUse();

                        if (veryVerbose) {
                            T_; T_; T_; P(X);
                        }

                        ASSERTV(INIT_LINE, LINE, i, ti,
                                                    checkIntegrity(X, LENGTH));
                        ASSERTV(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                        ASSERTV(INIT_LINE, LINE, i, ti,
                                                  A == expectedBlocks(LENGTH));

                        for (const_iterator j = X.begin(); j != X.end(); ++j) {
                            ASSERTV(INIT_LINE,LINE, i, ti, VALUE == *j);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERT(0 == oa.numMismatches());
                    ASSERT(0 == oa.numBlocksInUse());
                }
            }
        }
    }
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver2<TYPE,ALLOC>::test13_assignRange(const CONTAINER&)
{
    TestValues VALUES("ABCDEFGH");
    const int  NUM_VALUES = 8;

    // Note that aix gives warnings for 'const TYPE& X = TYPE()' for any type
    // that has a private copy c'tor.  This will work for the set of 'TYPE's we
    // actually test.

    TYPE DEFAULT_VALUE;
    if (bsl::is_trivially_default_constructible<TYPE>::value) {
#if defined(BSLS_PLATFORM_CMP_MSVC)
// False warning about memory leak (constructor *is* trivial, placement new)
#pragma warning( push )
#pragma warning( disable : 4291 )
#endif
        new (bsls::Util::addressOf(DEFAULT_VALUE)) TYPE();
#if defined(BSLS_PLATFORM_CMP_MSVC)
// Turn back on locally disabled warnings
#pragma warning( pop )
#endif
    }

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0 },
        { L_,        1 },
        { L_,        2 },
        { L_,        3 },
        { L_,        4 },
        { L_,        5 },
        { L_,        6 },
        { L_,        7 },
        { L_,        8 },
        { L_,        9 },
        { L_,       11 },
        { L_,       12 },
        { L_,       14 },
        { L_,       15 },
        { L_,       16 },
        { L_,       17 }
    };
    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // container spec
    } U_DATA[] = {
        //line  spec                   length
        //----  -------------------    ------
        { L_,   ""                  }, // 0
        { L_,   "A"                 }, // 1
        { L_,   "AB"                }, // 2
        { L_,   "ABC"               }, // 3
        { L_,   "ABCD"              }, // 4
        { L_,   "ABCDE"             }, // 5
        { L_,   "ABCDEAB"           }, // 7
        { L_,   "ABCDEABC"          }, // 8
        { L_,   "ABCDEABCD"         }, // 9
        { L_,   "ABCDEABCDEABCDE"   }, // 15
        { L_,   "ABCDEABCDEABCDEA"  }, // 16
        { L_,   "ABCDEABCDEABCDEAB" }  // 17
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    if (verbose) printf("\tUsing 'CONTAINER::const_iterator'.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        ALLOC                xoa(&oa);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf(" using default value.\n");
            }

            Obj        mX(INIT_LENGTH, VALUES[i % NUM_VALUES], xoa);
            const Obj& X = mX;

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec_p;
                const size_t  LENGTH = std::strlen(SPEC);

                CONTAINER mU(SPEC);  const CONTAINER& U = mU;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                mX.assign(U.begin(), U.end());
                const Int64 A = oa.numBlocksInUse();

                if (veryVerbose) { T_; T_; T_; P(X); }

                ASSERTV(INIT_LINE, LINE, i, ti, checkIntegrity(X, LENGTH));
                ASSERTV(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                ASSERTV(INIT_LINE, LINE, i, ti, A == expectedBlocks(LENGTH));

                Obj mY;  const Obj& Y = gg(&mY, SPEC);

                ASSERTV(INIT_LINE, LINE, i, ti, Y == X);
            }
        }
        ASSERT(0 == oa.numMismatches());
        ASSERT(0 == oa.numBlocksInUse());
    }

    if (verbose) printf("\tWith exceptions.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        ALLOC                xoa(&oa);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf(" using default value.\n");
            }

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec_p;
                const size_t  LENGTH = std::strlen(SPEC);

                CONTAINER mU(SPEC);  const CONTAINER& U = mU;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mY;  const Obj& Y = gg(&mY, SPEC);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const Int64 AL = oa.allocationLimit();
                    oa.setAllocationLimit(-1);

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, xoa); const Obj& X = mX;

                    oa.setAllocationLimit(AL);

                    mX.assign(U.begin(), U.end());  // test here
                    const Int64 A = oa.numBlocksInUse();

                    if (veryVerbose) {
                        T_; T_; T_; P(X);
                    }

                    ASSERTV(INIT_LINE, LINE, i, ti, checkIntegrity(X, LENGTH));
                    ASSERTV(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                    ASSERTV(INIT_LINE, LINE, i, ti,
                                                  A == expectedBlocks(LENGTH));
                    ASSERTV(INIT_LINE, LINE, i, ti, Y == X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(oa.numMismatches(),  0 == oa.numMismatches());
                ASSERTV(oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test13_assignRange()
{
    // ------------------------------------------------------------------------
    // TESTING INITIAL-LENGTH AND RANGE ASSIGNMENT
    //
    // Concerns:
    //: 1 That the initial value is correct.
    //:
    //: 2 That the initial range is correctly imported if the initial
    //:   'FWD_ITER' is an input iterator.
    //:
    //: 3 That the constructor is exception neutral w.r.t. memory allocation.
    //:
    //: 4 That the internal memory management system is hooked up properly so
    //:   that *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 5 The previous value is freed properly.
    //
    // Plan:
    //: 1 For the assignment we will create objects of varying sizes containing
    //:   default values for type T, and then assign different 'value' as
    //:   argument.  Perform the above tests:
    //:   o From the parameterized 'CONTAINER::const_iterator'.
    //:   o In the presence of exceptions during memory allocations using a
    //:     'bslma_TestAllocator' and varying its *allocation* *limit*.
    //:
    //: 2 Use basic accessors to verify:
    //:   o size
    //:   o capacity
    //:   o element value at each index position { 0 .. length - 1 }.
    //
    // Note that we relax the concerns about memory consumption, since this is
    // implemented as 'erase + insert', and insert will be tested more
    // completely in test case 17.
    //
    // Testing:
    //   template <class Iter> assign(Iter first, Iter last);
    // ------------------------------------------------------------------------

    if (verbose) printf("... with an arbitrary input iterator.\n");
    test13_assignRange(InputSeq<TYPE>());

    if (verbose) printf("... with an arbitrary random-access iterator.\n");
    test13_assignRange(RandSeq<TYPE>());
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test12_initialLengthConstructorDefault()
{
    // ------------------------------------------------------------------------
    // TESTING INITIAL-LENGTH CONSTRUCTORS
    //
    // Concerns:
    //: 1 The initial value is correct.
    //:
    //: 2 The constructor is exception neutral w.r.t. memory allocation.
    //:
    //: 3 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //
    // Plan:
    //: 1 We will create objects of varying sizes.
    //
    // Testing:
    //   list(size_type n);
    // ------------------------------------------------------------------------

    // Note that aix gives warnings for 'const TYPE& X = TYPE()' for any type
    // that has a private copy c'tor.  This will work for the set of 'TYPE's we
    // actually test.

    TYPE DEFAULT_VALUE;
    if (bsl::is_trivially_default_constructible<TYPE>::value) {
#if defined(BSLS_PLATFORM_CMP_MSVC)
// False warning about memory leak (constructor *is* trivial, placement new)
#pragma warning( push )
#pragma warning( disable : 4291 )
#endif
        new (bsls::Util::addressOf(DEFAULT_VALUE)) TYPE();
#if defined(BSLS_PLATFORM_CMP_MSVC)
// Turn back on locally disabled warnings
#pragma warning( pop )
#endif
    }

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0 },
        { L_,        1 },
        { L_,        2 },
        { L_,        3 },
        { L_,        4 },
        { L_,        5 },
        { L_,        6 },
        { L_,        7 },
        { L_,        8 },
        { L_,        9 },
        { L_,       11 },
        { L_,       12 },
        { L_,       14 },
        { L_,       15 },
        { L_,       16 },
        { L_,       17 },
        { L_,       31 },
        { L_,       32 },
        { L_,       33 },
        { L_,       63 },
        { L_,       64 },
        { L_,       65 }
    };
    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    if (verbose) printf("Testing TYPE: %s initial-length ctor "
                        "with default initial value.\n",
                        bsls::NameOf<TYPE>().name());

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int    LINE   = DATA[ti].d_lineNum;
        const size_t LENGTH = DATA[ti].d_length;

        if (verbose) { P_(LINE) P_(LENGTH) P(DEFAULT_VALUE); }

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj *objPtr = new (fa) Obj(LENGTH);

        ASSERTV(LINE, sizeof(Obj) == fa.numBytesInUse());

        Obj& mX = *objPtr;  const Obj& X = mX;

        const Int64 AA = da.numBlocksTotal();
        const Int64  A = da.numBlocksInUse();

        ASSERTV(LINE, ti, expectedBlocks(LENGTH) == AA);
        ASSERTV(LINE, ti, expectedBlocks(LENGTH) == A);

        ASSERTV(LINE, ti, checkIntegrity(X, LENGTH));
        ASSERTV(LINE, ti, X.size(), LENGTH == X.size());

        if (k_IS_DEFAULT_CONSTRUCTIBLE) {
            for (int j = 0; j < static_cast<int>(LENGTH); ++j) {
                ASSERTV(LINE, ti, j, DEFAULT_VALUE == nthElem(X,j));
            }
        }

        // Reclaim dynamically allocated object under the test.
        fa.deleteObject(objPtr);

        // Verify all memory is released on object destruction.
        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        ASSERTV(LINE, fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
    }
}


template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test12_initialLengthConstructorNonDefault()
{
    // ------------------------------------------------------------------------
    // TESTING INITIAL-LENGTH CONSTRUCTORS
    //
    // Concerns:
    //: 1 The initial value is correct.
    //:
    //: 2 The constructor is exception neutral w.r.t. memory allocation.
    //:
    //: 3 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 4 TBD: The C++0x move constructor moves value and allocator correctly,
    //:   and without performing any allocation.
    //:
    //: 5 Constructing a list with 'n' copies of value 'v' selects the correct
    //:   overload when 'n' and 'v' are identical arithmetic types (i.e., the
    //:   iterator-range overload is not selected).
    //:
    //: 6 Constructing a list with 'n' copies of value 'v' selects the correct
    //:   overload when 'v' is a pointer type and 'n' is a null pointer literal
    //:   ,'0'.  (i.e., the iterator-range overload is not selected).
    //
    // Plan:
    //: 1 For the constructor we will create objects of varying sizes with
    //:   different 'value' as argument.  Test first with the default value for
    //:   type T, and then test with different values.  Perform the above
    //:   tests:
    //:   o With and without passing in an allocator.
    //:   o In the presence of exceptions during memory allocations using a
    //:     'bslma_TestAllocator' and varying its *allocation* *limit*.
    //:   o Where the object is constructed with an object allocator, and
    //:     neither of global and default allocator is used to supply memory.
    //:
    //: 2 Use basic accessors to verify
    //:   o size
    //:   o allocator
    //:   o element value at each iterator position { begin() .. end() }.
    //:
    //: 3 As for concern 4, we simply move-construct each value into a new list
    //:   and check that the value, and allocator are as expected, and that no
    //:   allocation was performed.
    //:
    //: 4 For concerns 5 and 6, construct a list with 2 elements of arithmetic
    //:   or pointer types and verify that it compiles and that the resultant
    //:   list contains the expected values.
    //
    // Testing:
    //   list(size_type n);
    //   list(size_type n, const T& value = T(), const A& a = A());
    // ------------------------------------------------------------------------
    TestValues VALUES("ABCDEFGH");
    const int  NUM_VALUES = 8;

    // Note that aix gives warnings for 'const TYPE& X = TYPE()' for any type
    // that has a private copy c'tor.  This will work for the set of 'TYPE's we
    // actually test.

    TYPE DEFAULT_VALUE;
    if (bsl::is_trivially_default_constructible<TYPE>::value) {
#if defined(BSLS_PLATFORM_CMP_MSVC)
// False warning about memory leak (constructor *is* trivial, placement new)
#pragma warning( push )
#pragma warning( disable : 4291 )
#endif
        new (bsls::Util::addressOf(DEFAULT_VALUE)) TYPE();
#if defined(BSLS_PLATFORM_CMP_MSVC)
// Turn back on locally disabled warnings
#pragma warning( pop )
#endif
    }

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0 },
        { L_,        1 },
        { L_,        2 },
        { L_,        3 },
        { L_,        4 },
        { L_,        5 },
        { L_,        6 },
        { L_,        7 },
        { L_,        8 },
        { L_,        9 },
        { L_,       11 },
        { L_,       12 },
        { L_,       14 },
        { L_,       15 },
        { L_,       16 },
        { L_,       17 },
        { L_,       31 },
        { L_,       32 },
        { L_,       33 },
        { L_,       63 },
        { L_,       64 },
        { L_,       65 }
    };
    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    if (verbose) printf("Testing TYPE: %s initial-length ctor "
                        "with default initial value.\n",
                        bsls::NameOf<TYPE>().name());

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int    LINE   = DATA[ti].d_lineNum;
        const size_t LENGTH = DATA[ti].d_length;

        if (verbose) { P_(LINE) P_(LENGTH) P(DEFAULT_VALUE); }

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            if (veryVerbose) { T_ T_ P(CONFIG) }

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                  objPtr = new (fa) Obj(LENGTH, DEFAULT_VALUE);
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  objPtr = new (fa) Obj(LENGTH, DEFAULT_VALUE, ALLOC(0));
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  ALLOC xsa(&sa);
                  objPtr = new (fa) Obj(LENGTH, DEFAULT_VALUE, xsa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(LINE, CONFIG, !"Bad allocator config.");
              } return;                                               // RETURN
            }
            ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

            Obj& mX = *objPtr;  const Obj& X = mX;

            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = CONFIG <= 'b' ? sa : da;

            const Int64 AA = oa.numBlocksTotal();
            const Int64  A = oa.numBlocksInUse();

            ASSERTV(LINE, ti, CONFIG, expectedBlocks(LENGTH) == AA);
            ASSERTV(LINE, ti, CONFIG, expectedBlocks(LENGTH) == A);

            ASSERTV(LINE, ti, CONFIG, checkIntegrity(X, LENGTH));
            ASSERTV(LINE, ti, CONFIG, X.size(), LENGTH == X.size());
            ASSERTV(LINE, ti, CONFIG, noa.numBlocksInUse(),
                    0 == noa.numBlocksInUse());

            if (k_IS_DEFAULT_CONSTRUCTIBLE) {
                for (int j = 0; j < static_cast<int>(LENGTH); ++j) {
                    ASSERTV(LINE, ti, j, DEFAULT_VALUE == nthElem(X,j));
                }
            }

            // Reclaim dynamically allocated object under test.
            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.
            ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                    0 == da.numBlocksInUse());
            ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                    0 == fa.numBlocksInUse());
            ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                    0 == sa.numBlocksInUse());
        }
    }

    if (verbose) printf("Testing TYPE: %s initial-length ctor "
                        "with initial value.\n",
                        bsls::NameOf<TYPE>().name());

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int    LINE   = DATA[ti].d_lineNum;
        const size_t LENGTH = DATA[ti].d_length;
        const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

        if (verbose) { P_(LINE) P_(LENGTH) P(VALUE); }

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            if (veryVerbose) { T_ T_ P(CONFIG) }

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                  objPtr = new (fa) Obj(LENGTH, VALUE);
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  objPtr = new (fa) Obj(LENGTH, VALUE, ALLOC(0));
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  ALLOC xsa(&sa);
                  objPtr = new (fa) Obj(LENGTH, VALUE, xsa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(LINE, CONFIG, !"Bad allocator config.");
              } return;                                               // RETURN
            }
            ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

            Obj& mX = *objPtr;  const Obj& X = mX;

            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = CONFIG <= 'b' ? sa : da;

            const Int64 AA = oa.numBlocksTotal();
            const Int64  A = oa.numBlocksInUse();

            ASSERTV(LINE, ti, CONFIG, expectedBlocks(LENGTH) == AA);
            ASSERTV(LINE, ti, CONFIG, expectedBlocks(LENGTH) == A);

            ASSERTV(LINE, ti, CONFIG, checkIntegrity(X, LENGTH));
            ASSERTV(LINE, ti, CONFIG, X.size(), LENGTH == X.size());
            ASSERTV(LINE, ti, CONFIG, noa.numBlocksInUse(),
                    0 == noa.numBlocksInUse());

            for (int j = 0; j < static_cast<int>(LENGTH); ++j) {
                ASSERTV(LINE, ti, j, VALUE == nthElem(X,j));
            }

            // Reclaim dynamically allocated object under test.
            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.
            ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                    0 == da.numBlocksInUse());
            ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                    0 == fa.numBlocksInUse());
            ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                    0 == sa.numBlocksInUse());
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("Testing TYPE: %s initial-length ctor "
                        "with injected exceptions.\n",
                        bsls::NameOf<TYPE>().name());

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int    LINE   = DATA[ti].d_lineNum;
        const size_t LENGTH = DATA[ti].d_length;
        const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

        if (verbose) { P_(LINE) P_(LENGTH) P(VALUE); }

        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
        ALLOC                xsa(&sa);

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
            Obj mX(LENGTH, VALUE, xsa);  const Obj& X = mX;

            ASSERTV(LINE, ti, checkIntegrity(X, LENGTH));
            ASSERTV(LINE, ti, LENGTH == X.size());

            for (int j = 0; j < static_cast<int>(LENGTH); ++j) {
                ASSERTV(LINE, ti, j, VALUE == nthElem(X,j));
            }

        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        const Int64 AA = sa.numBlocksTotal();
        const Int64  A = sa.numBlocksInUse();

        if (veryVerbose) { printf("\t\tAFTER : "); P_(AA); P(A);}

        // The number of allocations, 'ALLOCS', needed for successful
        // construction of a list of length 'LENGTH' is
        // 'expectedBlocks(LENGTH)', Because we are retrying on each exception,
        // the number of allocations by the time we succeed will be 'SUM(1 ..
        // ALLOCS)', which is easily computed as 'ALLOCS * (ALLOCS+1) / 2'.

        const Int64 ALLOCS = expectedBlocks(LENGTH);
        const Int64 TOTAL_ALLOCS = ALLOCS * (ALLOCS+1) / 2;

        ASSERTV(LINE, ti, TOTAL_ALLOCS == AA);
        ASSERTV(LINE, ti, 0            ==  A);
    }
#endif
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test12_initialLengthConstructor(bsl::true_type)
{
    test12_initialLengthConstructorDefault();
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test12_initialLengthConstructor(bsl::false_type)
{
    test12_initialLengthConstructorDefault();
    test12_initialLengthConstructorNonDefault();
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test12_initialLengthConstructor()
{
    typedef typename bsl::integral_constant<bool,
                bsl::is_same<bsltf::MoveOnlyAllocTestType, TYPE>::value ||
                bsl::is_same<bsltf::WellBehavedMoveOnlyAllocTestType,
                                             TYPE>::value>::type HasNoCopyCtor;

    test12_initialLengthConstructor(HasNoCopyCtor());
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver2<TYPE,ALLOC>::test12_constructorRange(const CONTAINER&)
{
    const int INPUT_ITERATOR_TAG =
          bsl::is_same<std::input_iterator_tag,
                       typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::value;
    (void) INPUT_ITERATOR_TAG;

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // specification string
    } DATA[] = {
        //line spec
        //---- -----------
        { L_,  ""          },
        { L_,  "A"         },
        { L_,  "AB"        },
        { L_,  "ABC"       },
        { L_,  "ABCD"      },
        { L_,  "ABCDE"     },
        { L_,  "ABCDEAB"   },
        { L_,  "ABCDEABC"  },
        { L_,  "ABCDEABCD" }
    };
    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    if (verbose) printf("\tWithout passing in an allocator.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = std::strlen(SPEC);

            if (verbose) { printf("\t\tCreating object of "); P(SPEC); }

            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            CONTAINER mU(SPEC);  const CONTAINER& U = mU;

            Obj mX(U.begin(), U.end());  const Obj& X = mX;

            if (veryVerbose) { T_; T_; P(X); }

            ASSERTV(LINE, ti, checkIntegrity(X, LENGTH));
            ASSERTV(LINE, ti, &da == X.get_allocator().mechanism());
            ASSERTV(LINE, ti, LENGTH == X.size());

            Obj mY;  const Obj& Y = gg(&mY, SPEC);

            ASSERTV(LINE, ti, Y == X);
        }
    }

    if (verbose) printf("\tWith passing in an allocator.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = std::strlen(SPEC);

            if (verbose) { printf("\t\tCreating object "); P(SPEC); }

            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);
            bslma::TestAllocatorMonitor  dam(&da);

            CONTAINER mU(SPEC);  const CONTAINER& U = mU;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            ALLOC                xoa(&oa);

            dam.reset();

            Obj mX(U.begin(), U.end(), xoa);  const Obj& X = mX;

            const Int64 AA = oa.numBlocksTotal();
            const Int64  A = oa.numBlocksInUse();

            if (veryVerbose) {
                T_; T_; P(X);
                T_; T_; P_(AA); P(A);
            }

            ASSERTV(dam.isTotalSame());
            ASSERTV(LINE, ti, checkIntegrity(X, LENGTH));
            ASSERTV(LINE, ti, LENGTH == X.size());
            ASSERTV(LINE, ti, xoa == X.get_allocator());
            ASSERTV(LINE, ti, AA, AA == expectedBlocks(LENGTH));
            ASSERTV(LINE, ti, A,  A  == expectedBlocks(LENGTH));

            Obj mY; const Obj& Y = gg(&mY, SPEC);

            ASSERTV(LINE, ti, Y == X);
        }
    }

    if (verbose) printf("\tWith passing an allocator and checking for "
                        "allocation exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = std::strlen(SPEC);

            if (verbose) { printf("\t\tCreating object of "); P(SPEC); }

            CONTAINER mU(SPEC);  const CONTAINER& U = mU;

            Obj mY;  const Obj& Y = gg(&mY, SPEC);

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            ALLOC                xoa(&oa);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                Obj mX(U.begin(), U.end(), xoa); const Obj& X = mX;

                if (veryVerbose) { T_; T_; P(X); }

                ASSERTV(LINE, ti, checkIntegrity(X, LENGTH));
                ASSERTV(LINE, ti, LENGTH == X.size());
                ASSERTV(LINE, ti, Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const Int64 AA = oa.numBlocksTotal();
            const Int64  A = oa.numBlocksInUse();

            if (veryVerbose) { printf("\t\tAfter : "); P_(AA); P(A);}

            // The number of allocations, 'ALLOCS', needed for successful
            // construction of a list of length 'LENGTH' is
            // 'expectedBlocks(LENGTH)'.  Because we are retrying on each
            // exception, the number of allocations by the time we succeed will
            // be 'SUM(1 .. ALLOCS)', which is easily computed as
            // 'ALLOCS * (ALLOCS+1) / 2'.

            const Int64 ALLOCS = expectedBlocks(LENGTH);
#ifdef BDE_BUILD_TARGET_EXC
            const Int64 TOTAL_ALLOCS = ALLOCS * (ALLOCS+1) / 2;
#else
            const Int64 TOTAL_ALLOCS = ALLOCS;
#endif
            ASSERTV(LINE, ti, TOTAL_ALLOCS == AA);
            ASSERTV(LINE, ti, 0 == A);

            ASSERTV(LINE, ti, 0 == oa.numBlocksInUse());
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::test12_constructorRange()
{
    // ------------------------------------------------------------------------
    // TESTING INITIAL-LENGTH, RANGE CONSTRUCTORS:
    //
    // Concerns:
    //: 1 That the initial value is correct.
    //:
    //: 2 That the initial range is correctly imported if the initial
    //:   'FWD_ITER' is an input iterator.
    //:
    //: 3 That the constructor is exception neutral w.r.t. memory allocation.
    //:
    //: 4 That the internal memory management system is hooked up properly so
    //:   that *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //
    // Plan:
    //: 1 We will create objects of varying sizes containing default values,
    //:   and insert a range containing distinct values as argument.  Perform
    //:   the above tests:
    //:   o From the parameterized 'CONTAINER::const_iterator'.
    //:   o With and without passing in an allocator.
    //:   o In the presence of exceptions during memory allocations using a
    //:     'bslma_TestAllocator' and varying its *allocation* *limit*.
    //:
    //: 2 Use basic accessors to verify
    //:   o size
    //:   o element value at each index position { 0 .. length - 1 }.
    //
    // Note that this does NOT test the 'std::initializer_list' c'tor, which is
    // tested in case 33.
    //
    // Testing:
    //   template <class InputIter>
    //     list(InputIter first, InputIter last, const A& a = A());
    // ------------------------------------------------------------------------

    if (veryVerbose) printf("... with an arbitrary input iterator.\n");

    test12_constructorRange(InputSeq<TYPE>());

    if (veryVerbose)
        printf("... with an arbitrary random-access iterator.\n");

    test12_constructorRange(RandSeq<TYPE>());
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::list' cannot be deduced from the constructor parameters.
    //..
    // list()
    // list(size_t)
    // list(ALLOC)
    // list(size_t, ALLOC)
    //..

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    static void SimpleConstructors ()
        // Test that constructing a 'bsl::list' from various combinations of
        // arguments deduces the correct type.
        //..
        // list(const list&  l)        -> decltype(l)
        // list(const list&  l, ALLOC) -> decltype(l)
        // list(      list&& l)        -> decltype(l)
        // list(      list&& l, ALLOC) -> decltype(l)
        // list(size_type, VALUE_TYPE)        -> list<VALUE_TYPE>
        // list(size_type, VALUE_TYPE, ALLOC) -> list<VALUE_TYPE, ALLOC>
        // list(iter, iter)        -> list<iter::VALUE_TYPE>
        // list(iter, iter, ALLOC) -> list<iter::VALUE_TYPE, ALLOC>
        // list(initializer_list<T>)        -> list<T>
        // list(initializer_list<T>, ALLOC) -> list<T>
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        bsl::list<int> l1;
        bsl::list      l1a(l1);
        ASSERT_SAME_TYPE(decltype(l1a), bsl::list<int>);

        typedef float T2;
        bsl::list<T2> l2;
        bsl::list     l2a(l2, bsl::allocator<T2>());
        bsl::list     l2b(l2, a1);
        bsl::list     l2c(l2, a2);
        bsl::list     l2d(l2, bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(l2a), bsl::list<T2, bsl::allocator<T2>>);
        ASSERT_SAME_TYPE(decltype(l2b), bsl::list<T2, bsl::allocator<T2>>);
        ASSERT_SAME_TYPE(decltype(l2c), bsl::list<T2, bsl::allocator<T2>>);
        ASSERT_SAME_TYPE(decltype(l2d), bsl::list<T2, bsl::allocator<T2>>);

        bsl::list<short> l3;
        bsl::list        l3a(std::move(l3));
        ASSERT_SAME_TYPE(decltype(l3a), bsl::list<short>);

        typedef long double T4;
        bsl::list<T4> l4;
        bsl::list     l4a(std::move(l4), bsl::allocator<T4>());
        bsl::list     l4b(std::move(l4), a1);
        bsl::list     l4c(std::move(l4), a2);
        bsl::list     l4d(std::move(l4), bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(l4a), bsl::list<T4, bsl::allocator<T4>>);
        ASSERT_SAME_TYPE(decltype(l4b), bsl::list<T4, bsl::allocator<T4>>);
        ASSERT_SAME_TYPE(decltype(l4c), bsl::list<T4, bsl::allocator<T4>>);
        ASSERT_SAME_TYPE(decltype(l4d), bsl::list<T4, bsl::allocator<T4>>);

    // Turn off complaints about passing allocators in non-allocator positions
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -AM01
        bsl::list l5a(42, 3L);
        bsl::list l5b(42, bsl::allocator<long>{});
        bsl::list l5c(42, a1); // Deduce a list of 'bslma::Allocator *'
        ASSERT_SAME_TYPE(decltype(l5a), bsl::list<long>);
        ASSERT_SAME_TYPE(decltype(l5b), bsl::list<bsl::allocator<long>>);
        ASSERT_SAME_TYPE(decltype(l5c), bsl::list<bslma::Allocator *>);
    // BDE_VERIFY pragma: pop

        typedef double T6;
        bsl::list l6a(42, 3.0, bsl::allocator<T6>());
        bsl::list l6b(42, 3.0, a1);
        bsl::list l6c(42, 3.0, a2);
        bsl::list l6d(42, 3.0, std::allocator<T6>());
        ASSERT_SAME_TYPE(decltype(l6a), bsl::list<T6, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(l6b), bsl::list<T6, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(l6c), bsl::list<T6, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(l6d), bsl::list<T6, std::allocator<T6>>);

        typedef char T7;
        T7                      *p7b = nullptr;
        T7                      *p7e = nullptr;
        bsl::list<T7>::iterator  i7b;
        bsl::list<T7>::iterator  i7e;
        bsl::list                l7a(p7b, p7e);
        bsl::list                l7b(i7b, i7e);
        ASSERT_SAME_TYPE(decltype(l7a), bsl::list<T7>);
        ASSERT_SAME_TYPE(decltype(l7b), bsl::list<T7>);

        typedef unsigned short T8;
        T8                      *p8b = nullptr;
        T8                      *p8e = nullptr;
        bsl::list<T8>::iterator  i8b;
        bsl::list<T8>::iterator  i8e;

        bsl::list l8a(p8b, p8e, bsl::allocator<T8>());
        bsl::list l8b(p8b, p8e, a1);
        bsl::list l8c(p8b, p8e, a2);
        bsl::list l8d(p8b, p8e, std::allocator<T8>());
        bsl::list l8e(i8b, i8e, bsl::allocator<T8>());
        bsl::list l8f(i8b, i8e, a1);
        bsl::list l8g(i8b, i8e, a2);
        bsl::list l8h(i8b, i8e, std::allocator<T8>());
        ASSERT_SAME_TYPE(decltype(l8a), bsl::list<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(l8b), bsl::list<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(l8c), bsl::list<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(l8d), bsl::list<T8, std::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(l8e), bsl::list<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(l8f), bsl::list<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(l8g), bsl::list<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(l8h), bsl::list<T8, std::allocator<T8>>);

        bsl::list l9 {1LL, 2LL, 3LL, 4LL, 5LL};
        ASSERT_SAME_TYPE(decltype(l9), bsl::list<long long>);

        typedef long long T10;
        std::initializer_list<T10> il = {1LL, 2LL, 3LL, 4LL, 5LL};
        bsl::list                  l10a(il, bsl::allocator<T10>{});
        bsl::list                  l10b(il, a1);
        bsl::list                  l10c(il, a2);
        bsl::list                  l10d(il, std::allocator<T10>{});
        ASSERT_SAME_TYPE(decltype(l10a), bsl::list<T10, bsl::allocator<T10>>);
        ASSERT_SAME_TYPE(decltype(l10b), bsl::list<T10, bsl::allocator<T10>>);
        ASSERT_SAME_TYPE(decltype(l10c), bsl::list<T10, bsl::allocator<T10>>);
        ASSERT_SAME_TYPE(decltype(l10d), bsl::list<T10, std::allocator<T10>>);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests
// #define BSLSTL_LIST_COMPILE_FAIL_POINTER_IS_NOT_A_VALUE_TYPE
#if defined BSLSTL_LIST_COMPILE_FAIL_POINTER_IS_NOT_A_VALUE_TYPE
        bsl::List_Node<char> *lnp = nullptr; // pointer to random class type
        bsl::list l99(lnp, 3.0, a1);
        // This should fail to compile (pointer is not a size)
#endif
    }

#undef ASSERT_SAME_TYPE
};
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD


                     // ==================================
                     // class IncompleteTypeSupportChecker
                     // ==================================

class IncompleteTypeSupportChecker {
    // This class tests that 'bsl::list' can be instantiated using an
    // incomplete type and that methods within that incomplete type can
    // reference such a 'list'.  The interface completely copies the existing
    // 'bsl::list' interface and the only purpose of the functions is to call
    // the corresponding methods of the list parameterized by the incomplete
    // type.  Each method increases its own invocation counter so we can make
    // sure that each method is compiled and called.

    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil                      MoveUtil;
    typedef BloombergLP::bslmf::MovableRef<IncompleteTypeSupportChecker>
                                                                    MovableRef;

  public:
    // TYPES
    typedef bsl::list<IncompleteTypeSupportChecker> ListType;
    typedef ListType::value_type                    value_type;
    typedef ListType::size_type                     size_type;
    typedef ListType::allocator_type                allocator_type;
    typedef ListType::iterator                      iterator;
    typedef ListType::const_iterator                const_iterator;
    typedef ListType::reverse_iterator              reverse_iterator;
    typedef ListType::const_reverse_iterator        const_reverse_iterator;
    typedef ListType::reference                     reference;
    typedef ListType::const_reference               const_reference;

  private:
    // CLASS DATA
    static int const s_numFunctions;            // number of public methods
    static int       s_functionCallCounters[];  // number of times each
                                                // public method is called

    // DATA
    ListType d_list;  // underlying list parameterized with incomplete type

  public:
    // CLASS METHODS
    static void checkInvokedFunctions();
        // Check that all public functions have been called at least once.

    static void increaseFunctionCallCounter(std::size_t index);
        // Increase the call count of function with the specified 'index' by
        // one.

    static void resetFunctionCallCounters();
        // Reset the call count of each function to zero.

    // CREATORS
    IncompleteTypeSupportChecker()
        // Call 'bsl::list' default constructor implicitly.
    {
        s_functionCallCounters[0]++;
    }

    explicit IncompleteTypeSupportChecker(const allocator_type& basicAllocator)
        // Call 'bsl::list' constructor passing the specified 'basicAllocator'
        // as a parameter.
    : d_list(basicAllocator)
    {
        s_functionCallCounters[1]++;
    }

    explicit IncompleteTypeSupportChecker(size_type numElements)
        // Call 'bsl::list' constructor passing the specified 'numElements' as
        // a parameter.
    : d_list(numElements)
    {
        s_functionCallCounters[2]++;
    }

    IncompleteTypeSupportChecker(size_type             numElements,
                                 const allocator_type& basicAllocator)
        // Call 'bsl::list' constructor passing the specified 'numElements' and
        // 'basicAllocator' as parameters.
    : d_list(numElements, basicAllocator)
    {
        s_functionCallCounters[3]++;
    }

    IncompleteTypeSupportChecker(size_type             numElements,
                                 const value_type&     value,
                                 const allocator_type& basicAllocator
                                                            = allocator_type())
        // Call 'bsl::list' constructor passing the specified 'numElements',
        // 'value', and 'basicAllocator' as parameters.
    : d_list(numElements, value, basicAllocator)
    {
        s_functionCallCounters[4]++;
    }

    template <class INPUT_ITERATOR>
    IncompleteTypeSupportChecker(INPUT_ITERATOR        first,
                                 INPUT_ITERATOR        last,
                                 const allocator_type& basicAllocator
                                                            = allocator_type())
        // Call 'bsl::list' constructor passing the specified 'first',
        // 'last', and 'basicAllocator' as parameters.
    : d_list(first, last, basicAllocator)
    {
        s_functionCallCounters[5]++;
    }

    IncompleteTypeSupportChecker(const IncompleteTypeSupportChecker& original)
        // Call 'bsl::list' constructor passing the underlying list of the
        // specified 'original' as a parameter.
    : d_list(original.d_list)
    {
        s_functionCallCounters[6]++;
    }

    IncompleteTypeSupportChecker(
                            const IncompleteTypeSupportChecker& original,
                            const allocator_type&               basicAllocator)
        // Call 'bsl::list' constructor passing the underlying list of the
        // specified 'original' and the specified 'basicAllocator' as
        // parameters.
    : d_list(original.d_list, basicAllocator)
    {
        s_functionCallCounters[7]++;
    }

    IncompleteTypeSupportChecker(MovableRef original)
        // Call 'bsl::list' constructor passing the underlying list of the
        // specified movable 'original' as a parameter.
    : d_list(MoveUtil::move(MoveUtil::access(original).d_list))
    {
        s_functionCallCounters[8]++;
    }

    IncompleteTypeSupportChecker(MovableRef            original,
                                 const allocator_type& basicAllocator)
        // Call 'bsl::list' constructor passing the specified 'basicAllocator'
        // and passing the underlying list of the specified movable 'original'
        // as parameters.
    : d_list(MoveUtil::move(MoveUtil::access(original).d_list), basicAllocator)
    {
        s_functionCallCounters[9]++;
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    IncompleteTypeSupportChecker(
           std::initializer_list<value_type> values,
           const allocator_type&             basicAllocator = allocator_type())
        // Call 'bsl::list' constructor passing the specified 'values' and
        // 'basicAllocator as parameters.
    : d_list(values, basicAllocator)
    {
        s_functionCallCounters[10]++;
    }
#endif

    ~IncompleteTypeSupportChecker()
        // Call 'bsl::list' destructor.
    {
        s_functionCallCounters[11]++;
    }

    // MANIPULATORS
    IncompleteTypeSupportChecker& operator=(
                                       const IncompleteTypeSupportChecker& rhs)
        // Call the assignment operator of 'bsl::list' passing the underlying
        // list of the specified 'rhs' as a parameter, and return a reference
        // providing modifiable access to this object.
    {
        s_functionCallCounters[12]++;
        d_list = rhs.d_list;
        return *this;
    }

    IncompleteTypeSupportChecker& operator=(MovableRef rhs)
        // Call the assignment operator of 'bsl::list' passing the underlying
        // list of the specified movable 'rhs' as a parameter, and return a
        // reference providing modifiable access to this object.
    {
        s_functionCallCounters[13]++;
        d_list = MoveUtil::move(MoveUtil::access(rhs).d_list);
        return *this;
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    IncompleteTypeSupportChecker& operator=(
                                      std::initializer_list<value_type> values)
        // Call the assignment operator of 'bsl::list' passing the specified
        // 'values' as a parameter, and return a reference providing modifiable
        // access to this object.
    {
        s_functionCallCounters[14]++;
        d_list = values;
        return *this;
    }
#endif

    template <class INPUT_ITERATOR>
    void assign(INPUT_ITERATOR first, INPUT_ITERATOR last)
        // Call the 'assign' method of 'bsl::list' passing the specified
        // 'first' and 'last' as parameters.
    {
        s_functionCallCounters[15]++;
        d_list.assign(first, last);
    }

    void assign(size_type numElements, const value_type& value)
        // Call the 'assign' method of 'bsl::list' passing the specified
        // 'numElements' and 'value' as parameters.
    {
        s_functionCallCounters[16]++;
        d_list.assign(numElements, value);
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void assign(std::initializer_list<value_type> values)
        // Call the 'assign' method of 'bsl::list' passing the specified
        // 'values' as a parameter.
    {
        s_functionCallCounters[17]++;
        d_list.assign(values);
    }
#endif

    iterator begin()
        // Return the result of calling the 'begin' manipulator of 'bsl::list'.
    {
        s_functionCallCounters[18]++;
        return d_list.begin();
    }

    iterator end()
        // Return the result of calling the 'end' manipulator of 'bsl::list'.
    {
        s_functionCallCounters[19]++;
        return d_list.end();
    }

    reverse_iterator rbegin()
        // Return the result of calling the 'rbegin' manipulator of
        // 'bsl::list'.
    {
        s_functionCallCounters[20]++;
        return d_list.rbegin();
    }

    reverse_iterator rend()
        // Return the result of calling the 'rend' manipulator of 'bsl::list'.
    {
        s_functionCallCounters[21]++;
        return d_list.rend();
    }

    void clear()
        // Call the 'clear' method of 'bsl::list'.
    {
        s_functionCallCounters[22]++;
        d_list.clear();
    }

    void resize(size_type newSize)
        // Call the 'resize' method of 'bsl::list' passing the specified
        // 'newSize' as a parameter.
    {
        s_functionCallCounters[23]++;
        d_list.resize(newSize);
    }

    void resize(size_type newSize, const value_type& value)
        // Call the 'resize' method of 'bsl::list' passing the specified
        // 'newSize' and 'value' as parameters.
    {
        s_functionCallCounters[24]++;
        d_list.resize(newSize, value);
    }

    reference back()
        // Return the result of calling the 'back' manipulator of 'bsl::list'.
    {
        s_functionCallCounters[25]++;
        return d_list.back();
    }

    reference front()
        // Return the result of calling the 'front' manipulator of 'bsl::list'.
    {
        s_functionCallCounters[26]++;
        return d_list.front();
    }

    void pop_back()
        // Call the 'pop_back' method of 'bsl::list'.
    {
        s_functionCallCounters[27]++;
        d_list.pop_back();
    }

    void pop_front()
        // Call the 'pop_front' method of 'bsl::list'.
    {
        s_functionCallCounters[28]++;
        d_list.pop_front();
    }

    iterator erase(const_iterator position)
        // Call the 'erase' method of 'bsl::list' passing the specified
        // 'position' as a parameter, and return the result.
    {
        s_functionCallCounters[29]++;
        return d_list.erase(position);
    }

    iterator erase(const_iterator dstBegin, const_iterator dstEnd)
        // Call the 'erase' method of 'bsl::list' passing the specified
        // 'dstBegin' and 'dstEnd' as parameters, and return the result.
    {
        s_functionCallCounters[30]++;
        return d_list.erase(dstBegin, dstEnd);
    }

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    reference emplace_back(ARGS&&... arguments)
        // Call the 'emplace_back' method of 'bsl::list' passing the specified
        // 'arguments' as a parameter, and return the result.
    {
        s_functionCallCounters[31]++;
        return d_list.emplace_back(
            BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    }
#else
    template <class ARGS_01,
              class ARGS_02>
    reference emplace_back(
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) arguments_02)
        // Call the 'emplace_back' method of 'bsl::list' passing the specified
        // 'arguments_01' and 'arguments_02' as parameters, and return the
        // result.
    {
        s_functionCallCounters[31]++;
        return d_list.emplace_back(
                         BSLS_COMPILERFEATURES_FORWARD(ARGS_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(ARGS_02, arguments_02));
    }
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    reference emplace_front(ARGS&&... arguments)
        // Call the 'emplace_front' method of 'bsl::list' passing the specified
        // 'arguments' as a parameter, and return the result.
    {
        s_functionCallCounters[32]++;
        return d_list.emplace_front(
            BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    }
#else
    template <class ARGS_01,
              class ARGS_02>
    reference emplace_front(
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) arguments_02)
        // Call the 'emplace_front' method of 'bsl::list' passing the specified
        // 'arguments_01' and 'arguments_02' as parameters, and return the
        // result.
    {
        s_functionCallCounters[32]++;
        return d_list.emplace_front(
                         BSLS_COMPILERFEATURES_FORWARD(ARGS_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(ARGS_02, arguments_02));
    }
#endif

    void push_back(const value_type& value)
        // Call the 'push_back' method of 'bsl::list' passing the specified
        // 'value' as a parameter.
    {
        s_functionCallCounters[33]++;
        d_list.push_back(value);
    }

    void push_back(MovableRef value)
        // Call the 'push_back' method of 'bsl::list' passing the specified
        // movable 'value' as a parameter.
    {
        s_functionCallCounters[34]++;
        IncompleteTypeSupportChecker& lvalue = value;
        d_list.push_back(MoveUtil::move(lvalue));
    }

    void push_front(const value_type& value)
        // Call the 'push_front' method of 'bsl::list' passing the specified
        // 'value' as a parameter.
    {
        s_functionCallCounters[35]++;
        d_list.push_front(value);
    }

    void push_front(MovableRef value)
        // Call the 'push_front' method of 'bsl::list' passing the specified
        // movable 'value' as a parameter.
    {
        s_functionCallCounters[36]++;
        IncompleteTypeSupportChecker& lvalue = value;
        d_list.push_front(MoveUtil::move(lvalue));
    }

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    iterator emplace(const_iterator position, ARGS&&... arguments)
        // Call the 'emplace' method of 'bsl::list' passing the specified
        // 'position' and 'arguments' as parameters, and return the result.
    {
        s_functionCallCounters[37]++;
        return d_list.emplace(
                            position,
                            BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    }
#else
    template <class ARGS_01,
              class ARGS_02>
    iterator emplace(const_iterator                             position,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) arguments_01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) arguments_02)
        // Call the 'emplace' method of 'bsl::list' passing the specified
        // 'position', 'arguments_01' and 'arguments_02' as parameters, and
        // return the result.
    {
        s_functionCallCounters[37]++;
        return d_list.emplace(
                         position,
                         BSLS_COMPILERFEATURES_FORWARD(ARGS_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(ARGS_02, arguments_02));
    }
#endif

    iterator insert(const_iterator dstPosition, const value_type& value)
        // Call the 'insert' method of 'bsl::list' passing the specified
        // 'dstPosition' and 'value' as parameters, and return the result.
    {
        s_functionCallCounters[38]++;
        return d_list.insert(dstPosition, value);
    }

    iterator insert(const_iterator dstPosition, MovableRef value)
        // Call the 'insert' method of 'bsl::list' passing the specified
        // 'dstPosition' and the specified movable 'value' as parameters, and
        // return the result.
    {
        s_functionCallCounters[39]++;
        IncompleteTypeSupportChecker& lvalue = value;
        return d_list.insert(dstPosition, MoveUtil::move(lvalue));
    }

    iterator insert(const_iterator    dstPosition,
                    size_type         numElements,
                    const value_type& value)
        // Call the 'insert' method of 'bsl::list' passing the specified
        // 'dstPosition', 'numElements' and 'value' as parameters, and return
        // the result.
    {
        s_functionCallCounters[40]++;
        return d_list.insert(dstPosition, numElements, value);
    }

    template <class INPUT_ITERATOR>
    iterator insert(const_iterator dstPosition,
                    INPUT_ITERATOR first,
                    INPUT_ITERATOR last)
        // Call the 'insert' method of 'bsl::list' passing the specified
        // 'dstPosition', 'first' and 'last' as parameters, and return the
        // result.
    {
        s_functionCallCounters[41]++;
        return d_list.insert(dstPosition, first, last);
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    iterator insert(const_iterator                    dstPosition,
                    std::initializer_list<value_type> values)
        // Call the 'insert' method of 'bsl::list' passing the specified
        // 'dstPosition' and 'values' as parameters, and return the result.
    {
        s_functionCallCounters[42]++;
        return d_list.insert(dstPosition, values);
    }
#endif

    void merge(IncompleteTypeSupportChecker& other)
        // Call the 'merge' method of 'bsl::list' passing the specified 'other'
        // as a parameter.
    {
        s_functionCallCounters[43]++;
        d_list.merge(other.d_list);
    }

    void merge(MovableRef other)
        // Call the 'merge' method of 'bsl::list' passing the specified movable
        // 'other' as a parameter.
    {
        s_functionCallCounters[44]++;
        d_list.merge(MoveUtil::move(MoveUtil::access(other).d_list));
    }

    template <class COMPARE>
    void merge(IncompleteTypeSupportChecker& other, COMPARE comparator)
        // Call the 'merge' method of 'bsl::list' passing the specified 'other'
        // and 'comparator' as parameters.
    {
        s_functionCallCounters[45]++;
        d_list.merge(other.d_list, comparator);
    }

    template <class COMPARE>
    void merge(MovableRef other, COMPARE comparator)
        // Call the 'merge' method of 'bsl::list' passing the specified movable
        // 'other' and 'comparator' as parameters.
    {
        s_functionCallCounters[46]++;
        d_list.merge(MoveUtil::move(MoveUtil::access(other).d_list),
                     comparator);
    }

    void remove(const value_type& value)
        // Call the 'remove' method of 'bsl::list' passing the specified
        // 'value' as a parameter.
    {
        s_functionCallCounters[47]++;
        d_list.remove(value);
    }

    template <class PREDICATE>
    void remove_if(PREDICATE predicate)
        // Call the 'remove_if' method of 'bsl::list' passing the specified
        // 'predicate' as a parameter.
    {
        s_functionCallCounters[48]++;
        d_list.remove_if(predicate);
    }

    void reverse()
        // Call the 'reverse' method of 'bsl::list'.
    {
        s_functionCallCounters[49]++;
        d_list.reverse();
    }

    void sort()
        // Call the 'sort' method of 'bsl::list'.
    {
        s_functionCallCounters[50]++;
        d_list.sort();
    }

    template <class COMPARE>
    void sort(COMPARE comparator)
        // Call the 'sort' method of 'bsl::list' passing the specified
        // 'comparator' as a parameter.
    {
        s_functionCallCounters[51]++;
        d_list.sort(comparator);
    }

    void splice(const_iterator dstPosition, IncompleteTypeSupportChecker& src)
        // Call the 'splice' method of 'bsl::list' passing the specified
        // 'dstPosition' and 'src' as parameters.
    {
        s_functionCallCounters[52]++;
        d_list.splice(dstPosition, src.d_list);
    }

    void splice(const_iterator dstPosition, MovableRef src)
        // Call the 'splice' method of 'bsl::list' passing the specified
        // 'dstPosition' and movable 'src' as parameters.
    {
        s_functionCallCounters[53]++;
        d_list.splice(dstPosition,
                      MoveUtil::move(MoveUtil::access(src).d_list));
    }

    void splice(const_iterator                dstPosition,
                IncompleteTypeSupportChecker& src,
                const_iterator                srcNode)
        // Call the 'splice' method of 'bsl::list' passing the specified
        // 'dstPosition', 'src' and 'srcNode' as parameters.
    {
        s_functionCallCounters[54]++;
        d_list.splice(dstPosition, src.d_list, srcNode);
    }

    void splice(const_iterator dstPosition,
                MovableRef     src,
                const_iterator srcNode)
        // Call the 'splice' method of 'bsl::list' passing the specified
        // 'dstPosition', movable 'src' and 'srcNode' as parameters.
    {
        s_functionCallCounters[55]++;
        d_list.splice(dstPosition,
                      MoveUtil::move(MoveUtil::access(src).d_list),
                      srcNode);
    }

    void splice(const_iterator                dstPosition,
                IncompleteTypeSupportChecker& src,
                const_iterator                first,
                const_iterator                last)
        // Call the 'splice' method of 'bsl::list' passing the specified
        // 'dstPosition', 'src', 'first' and 'last' as parameters.
    {
        s_functionCallCounters[56]++;
        d_list.splice(dstPosition, src.d_list, first, last);
    }

    void splice(const_iterator dstPosition,
                MovableRef     src,
                const_iterator first,
                const_iterator last)
        // Call the 'splice' method of 'bsl::list' passing the specified
        // 'dstPosition', movable 'src', 'first' and 'last' as parameters.
    {
        s_functionCallCounters[57]++;
        d_list.splice(dstPosition,
                      MoveUtil::move(MoveUtil::access(src).d_list),
                      first,
                      last);
    }

    void unique()
        // Call the 'unique' method of 'bsl::list'.
    {
        s_functionCallCounters[58]++;
        d_list.unique();
    }

    template <class EQ_PREDICATE>
    void unique(EQ_PREDICATE binaryPredicate)
        // Call the 'unique' method of 'bsl::list' passing the specified
        // 'binaryPredicate' as a parameter.
    {
        s_functionCallCounters[59]++;
        d_list.unique(binaryPredicate);
    }

    void swap(IncompleteTypeSupportChecker& other)
        // Call the 'swap' method of 'bsl::list' passing the specified 'other'
        // as a parameter.
    {
        s_functionCallCounters[60]++;
        d_list.swap(other.d_list);
    }

    // ACCESSORS
    const_iterator  begin() const
        // Return the result of calling the 'begin' accessor of 'bsl::list'.
    {
        s_functionCallCounters[61]++;
        return d_list.begin();
    }

    const_iterator cbegin() const
        // Return the result of calling the 'cbegin' accessor of 'bsl::list'.
    {
        s_functionCallCounters[62]++;
        return d_list.cbegin();
    }

    const_iterator end() const
        // Return the result of calling the 'end' accessor of 'bsl::list'.
    {
        s_functionCallCounters[63]++;
        return d_list.end();
    }

    const_iterator cend() const
        // Return the result of calling the 'cend' accessor of 'bsl::list'.
    {
        s_functionCallCounters[64]++;
        return d_list.cend();
    }

    const_reverse_iterator  rbegin() const
        // Return the result of calling the 'rbegin' accessor of 'bsl::list'.
    {
        s_functionCallCounters[65]++;
        return d_list.rbegin();
    }

    const_reverse_iterator crbegin() const
        // Return the result of calling the 'crbegin' accessor of 'bsl::list'.
    {
        s_functionCallCounters[66]++;
        return d_list.crbegin();
    }

    const_reverse_iterator  rend() const
        // Return the result of calling the 'rend' accessor of 'bsl::list'.
    {
        s_functionCallCounters[67]++;
        return d_list.rend();
    }

    const_reverse_iterator crend() const
        // Return the result of calling the 'crend' accessor of 'bsl::list'.
    {
        s_functionCallCounters[68]++;
        return d_list.crend();
    }

    bool empty() const
        // Return the result of calling the 'empty' method of 'bsl::list'.
    {
        s_functionCallCounters[69]++;
        return d_list.empty();
    }

    size_type max_size() const
        // Return the result of calling the 'max_size' method of 'bsl::list'.
    {
        s_functionCallCounters[70]++;
        return d_list.max_size();
    }

    size_type size() const
        // Return the result of calling the 'size' method of 'bsl::list'.
    {
        s_functionCallCounters[71]++;
        return d_list.size();
    }

    const_reference back() const
        // Return the result of calling the 'back' method of 'bsl::list'.
    {
        s_functionCallCounters[72]++;
        return d_list.back();
    }

    const_reference front() const
        // Return the result of calling the 'back' method of 'bsl::list'.
    {
        s_functionCallCounters[73]++;
        return d_list.front();
    }

    allocator_type get_allocator() const
        // Return the result of calling the 'get_allocator' method of
        // 'bsl::list'.
    {
        s_functionCallCounters[74]++;
        return d_list.get_allocator();
    }

    const ListType& content() const;
        // Return a reference providing non-modifiable access to the underlying
        // list.
};

// FREE OPERATORS
bool operator==(const IncompleteTypeSupportChecker& lhs,
                const IncompleteTypeSupportChecker& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'IncompleteTypeSupportChecker'
    // objects 'lhs' and 'rhs' have the same value if their underlying lists
    // have the same value.

bool operator<(const IncompleteTypeSupportChecker& lhs,
               const IncompleteTypeSupportChecker& rhs);
    // Return 'true' if the value of the underlying list of the specified 'lhs'
    // is lexicographically less than that of the specified 'rhs', and 'false'
    // otherwise.

                     // ==============================
                     // class IncompleteTypeComparator
                     // ==============================

class IncompleteTypeComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of 'IncompleteTypeSupportChecker'
    // type.

  public:
    // ACCESSORS
    bool operator()(const IncompleteTypeSupportChecker& lhs,
                    const IncompleteTypeSupportChecker& rhs);
        // Return 'true' if the specified 'lhs' less than the specified 'rhs',
        // and 'false' otherwise.
};

                     // =====================================
                     // class IncompleteTypeEqualityPredicate
                     // =====================================

class IncompleteTypeEqualityPredicate {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of 'IncompleteTypeSupportChecker'
    // type.

  public:
    // ACCESSORS
    bool operator()(const IncompleteTypeSupportChecker& lhs,
                    const IncompleteTypeSupportChecker& rhs);
        // Return 'true' if the specified 'lhs' is equal to the specified
        // 'rhs', and 'false' otherwise.
};

                  // ----------------------------------
                  // class IncompleteTypeSupportChecker
                  // ----------------------------------
// CLASS DATA
const int IncompleteTypeSupportChecker::s_numFunctions = 75;
int       IncompleteTypeSupportChecker::s_functionCallCounters[s_numFunctions]
                                                                         = { };


// CLASS METHODS
void IncompleteTypeSupportChecker::checkInvokedFunctions()
{
    for (std::size_t i = 0; i < s_numFunctions; ++i) {
        const size_t INDEX = i;
        ASSERTV(INDEX, 0 < s_functionCallCounters[INDEX]);
    }
}

void IncompleteTypeSupportChecker::increaseFunctionCallCounter(
                                                             std::size_t index)
{
    s_functionCallCounters[index]++;
}

void IncompleteTypeSupportChecker::resetFunctionCallCounters()
{
    for (std::size_t i = 0; i < s_numFunctions; ++i) {
        s_functionCallCounters[i] = 0;
    }
}

// ACCESSORS
const IncompleteTypeSupportChecker::ListType&
IncompleteTypeSupportChecker::content() const
{
    return d_list;
}

// FREE OPERATORS
bool operator==(const IncompleteTypeSupportChecker& lhs,
                const IncompleteTypeSupportChecker& rhs)
{
    return lhs.content() == rhs.content();
}

bool operator<(const IncompleteTypeSupportChecker& lhs,
               const IncompleteTypeSupportChecker& rhs)
{
    return lhs.content() < rhs.content();
}
                     // ------------------------------
                     // class IncompleteTypeComparator
                     // ------------------------------

bool IncompleteTypeComparator::operator()(
                                       const IncompleteTypeSupportChecker& lhs,
                                       const IncompleteTypeSupportChecker& rhs)
{
    return lhs < rhs;
}
                     // -------------------------------------
                     // class IncompleteTypeEqualityPredicate
                     // -------------------------------------

bool IncompleteTypeEqualityPredicate::operator()(
                                       const IncompleteTypeSupportChecker& lhs,
                                       const IncompleteTypeSupportChecker& rhs)
{
    return (lhs == rhs);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? std::atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    // As part of our overall allocator testing strategy, we will create two
    // test allocators.

    // Default Test Allocator.
    bslma::TestAllocator         defaultAllocator("Default Allocator",
                                                  veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);
    defaultAllocator_p = &defaultAllocator;

    // Global Test Allocator.
    bslma::TestAllocator  globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);
    globalAllocator_p = &globalAllocator;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 37: {
        // --------------------------------------------------------------------
        // TESTING INCOMPLETE TYPE SUPPORT
        //
        // Concerns:
        //: 1 All public methods of 'bsl::list' parameterized with incomplete
        //:   type are successfully compiled.
        //
        // Plan:
        //: 1 Invoke each public method of the special test type
        //:   'IncompleteTypeSupportChecker'.
        //
        // Testing:
        //   INCOMPLETE TYPE SUPPORT
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING INCOMPLETE TYPE SUPPORT"
                            "\n===============================\n");

        bslma::TestAllocator                         da("default",
                                                        veryVeryVeryVerbose);
        bslma::TestAllocator                         sa("supplied",
                                                        veryVeryVeryVerbose);
        bsl::allocator<IncompleteTypeSupportChecker> defaultAllocator(&da);
        bsl::allocator<IncompleteTypeSupportChecker> suppliedAllocator(&sa);
        bslma::DefaultAllocatorGuard                 dag(&da);

        const IncompleteTypeSupportChecker::size_type initialSize = 5;

        IncompleteTypeSupportChecker::resetFunctionCallCounters();

        {
            IncompleteTypeSupportChecker        mIT;                       // 0
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(0 == IT.size());
        }

        {
            IncompleteTypeSupportChecker        mIT(suppliedAllocator);    // 1
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(suppliedAllocator == IT.get_allocator());
        }

        {
            IncompleteTypeSupportChecker        mIT(initialSize);          // 2
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(initialSize ==  IT.size());
        }

        {
            IncompleteTypeSupportChecker        mIT(initialSize,
                                                    suppliedAllocator);    // 3
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(initialSize       == IT.size()         );
            ASSERT(suppliedAllocator == IT.get_allocator());
        }

        {
            IncompleteTypeSupportChecker        mIT(
                                               initialSize,
                                               IncompleteTypeSupportChecker());
                                                                           // 4
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(initialSize == IT.size());
        }

        {
            IncompleteTypeSupportChecker        source[initialSize];
            IncompleteTypeSupportChecker        mIT(source,
                                                    source + initialSize);
            const IncompleteTypeSupportChecker& IT = mIT;                  // 5

            ASSERT(initialSize == IT.size());
        }

        {
            const IncompleteTypeSupportChecker  source(initialSize);
            IncompleteTypeSupportChecker        mIT(source);               // 6
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(initialSize == IT.size());
        }

        {
            const IncompleteTypeSupportChecker  source(initialSize);
            IncompleteTypeSupportChecker        mIT(source,
                                                    suppliedAllocator);    // 7
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(initialSize       == IT.size()         );
            ASSERT(suppliedAllocator == IT.get_allocator());
        }

        {
            IncompleteTypeSupportChecker        source(initialSize);
            IncompleteTypeSupportChecker        mIT(
                                    bslmf::MovableRefUtil::move(source));  // 8
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(initialSize == IT.size());
        }

        {
            IncompleteTypeSupportChecker        source(initialSize);
            IncompleteTypeSupportChecker        mIT(
                                           bslmf::MovableRefUtil::move(source),
                                           suppliedAllocator);             // 9
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(initialSize       == IT.size()         );
            ASSERT(suppliedAllocator == IT.get_allocator());
        }                                                                 // 11

        {
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
            std::initializer_list<IncompleteTypeSupportChecker> values;
            IncompleteTypeSupportChecker                        mIT(values);
                                                                          // 10
            const IncompleteTypeSupportChecker&                 IT = mIT;

            ASSERT(0 == IT.size());
#else
            IncompleteTypeSupportChecker::increaseFunctionCallCounter(10);
#endif
        }

        {
            const IncompleteTypeSupportChecker         source0;
            const IncompleteTypeSupportChecker         source1(initialSize);
            IncompleteTypeSupportChecker               source2;
            IncompleteTypeSupportChecker               source3[initialSize];
            IncompleteTypeSupportChecker               source4;
            IncompleteTypeSupportChecker               source5;
            IncompleteTypeSupportChecker               source6;
            IncompleteTypeSupportChecker               source7(initialSize);
            IncompleteTypeSupportChecker               source8(initialSize);
            IncompleteTypeSupportChecker               source9(initialSize);
            IncompleteTypeSupportChecker               source10(initialSize);
            IncompleteTypeSupportChecker               source11(initialSize);
            IncompleteTypeSupportChecker               source12(initialSize);
            IncompleteTypeSupportChecker               source13(initialSize);
            IncompleteTypeSupportChecker               source14(initialSize);

            IncompleteTypeComparator                   comparator;
            IncompleteTypeEqualityPredicate            equalityPredicate;

            IncompleteTypeSupportChecker        mIT;
            const IncompleteTypeSupportChecker& IT = mIT;

            IncompleteTypeSupportChecker *mR = &(mIT = source1);          // 12

            ASSERT(initialSize == IT.size());
            ASSERT(mR          == &mIT     );

            mR = &(mIT = bslmf::MovableRefUtil::move(source2));           // 13

            ASSERT(0  == IT.size());
            ASSERT(mR == &mIT     );

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
            std::initializer_list<IncompleteTypeSupportChecker> values;

            mR = &(mIT = values);                                         // 14

            ASSERT(0  == IT.size());
            ASSERT(mR == &mIT     );
#else
            IncompleteTypeSupportChecker::increaseFunctionCallCounter(14);
#endif

            mIT.assign(source3, source3 + initialSize);                   // 15

            ASSERT(initialSize == IT.size());

            mIT.assign(2 * initialSize, source1);                         // 16

            ASSERT(2 * initialSize == IT.size());

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
            mIT.assign(values);                                           // 17

            ASSERT(0  == IT.size());
#else
            IncompleteTypeSupportChecker::increaseFunctionCallCounter(17);
#endif

            mIT.assign(source3, source3);  // Empty range

            ASSERT(mIT.end()    == mIT.begin() );                         // 18
            ASSERT(mIT.begin()  == mIT.end()   );                         // 19
            ASSERT(mIT.rend()   == mIT.rbegin());                         // 20
            ASSERT(mIT.rbegin() == mIT.rend()  );                         // 21

            mIT.assign(initialSize, source1);
            ASSERT(initialSize == IT.size());

            mIT.clear();                                                  // 22

            ASSERT(0 == IT.size());

            mIT.resize(initialSize);                                      // 23

            ASSERT(initialSize == IT.size());

            mIT.resize(2 * initialSize, source1);                         // 24

            ASSERT(2 * initialSize == IT.size());

            mIT.back() = source2;                                         // 25

            ASSERT(source2 == IT.back());

            mIT.front() = source1;                                        // 26

            ASSERT(source1 == IT.front());

            mIT.pop_back();                                               // 27

            ASSERT(2 * initialSize - 1 == IT.size());

            mIT.pop_front();                                              // 28

            ASSERT(2 * initialSize - 2 == IT.size());

            mIT.erase(IT.cbegin());                                       // 29

            ASSERT(2 * initialSize - 3 == IT.size());

            mIT.erase(IT.cbegin(), ++IT.cbegin());                        // 30

            ASSERTV(2 * initialSize - 4 == IT.size());

            mR = &(mIT.emplace_back(initialSize,
                                    IncompleteTypeSupportChecker()));     // 31

            ASSERT(2 * initialSize - 3 == IT.size());
            ASSERT(source1             == IT.back());
            ASSERT(source1             == *mR      );

            mR = &(mIT.emplace_front(initialSize,
                                     IncompleteTypeSupportChecker()));    // 32

            ASSERT(2 * initialSize - 2 == IT.size() );
            ASSERT(source1             == IT.front());
            ASSERT(source1             == *mR       );

            mIT.push_back(source1);                                       // 33

            ASSERT(2 * initialSize - 1 == IT.size() );

            mIT.push_back(BloombergLP::bslmf::MovableRefUtil::move(source4));
                                                                          // 34
            ASSERT(2 * initialSize == IT.size() );

            mIT.push_front(source1);                                      // 35

            ASSERT(2 * initialSize + 1 == IT.size() );

            mIT.push_front(BloombergLP::bslmf::MovableRefUtil::move(source5));
                                                                          // 36
            ASSERT(2 * initialSize + 2 == IT.size() );

            IncompleteTypeSupportChecker::iterator mIter = mIT.emplace(
                                               mIT.cbegin(),
                                               initialSize,
                                               IncompleteTypeSupportChecker());
                                                                          // 37
            ASSERT(2 * initialSize + 3 == IT.size());
            ASSERT(IT.begin()          == mIter    );

            mIter = mIT.insert(mIT.cbegin(), source1);                    // 38

            ASSERT(2 * initialSize + 4 == IT.size());
            ASSERT(IT.begin()          == mIter    );

            mIter = mIT.insert(
                            mIT.cbegin(),
                            BloombergLP::bslmf::MovableRefUtil::move(source6));
                                                                          // 39
            ASSERT(2 * initialSize + 5 == IT.size());
            ASSERT(IT.begin()          == mIter    );

            mIter = mIT.insert(mIT.cbegin(), 5, source1);                 // 40

            ASSERT(2 * initialSize + 10 == IT.size());
            ASSERT(IT.begin()           == mIter    );

            mIter = mIT.insert(mIT.cbegin(), source3, source3 + initialSize);
                                                                          // 41
            ASSERT(2 * initialSize + 15 == IT.size());
            ASSERT(IT.begin()           == mIter    );

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
            mIter = mIT.insert(mIT.cbegin(), values);                     // 42

            ASSERT(2 * initialSize + 15 == IT.size());
            ASSERT(IT.begin()           == mIter    );
#else
            IncompleteTypeSupportChecker::increaseFunctionCallCounter(42);
#endif

            mIT.merge(source7);                                           // 43

            ASSERT(2 * initialSize + 20 == IT.size()     );
            ASSERT(0                    == source7.size());

            mIT.merge(BloombergLP::bslmf::MovableRefUtil::move(source8));
                                                                          // 44
            ASSERT(2 * initialSize + 25 == IT.size());

            mIT.merge(source9, comparator);                               // 45

            ASSERT(2 * initialSize + 30 == IT.size()     );
            ASSERT(0                    == source9.size());

            mIT.merge(BloombergLP::bslmf::MovableRefUtil::move(source10),
                      comparator);                                        // 46

            ASSERT(2 * initialSize + 35 == IT.size());

            mIT.remove(source1);                                          // 47

            ASSERT(2 * initialSize + 20 == IT.size());

            mIT.remove_if(EqPred<IncompleteTypeSupportChecker>(source0)); // 48

            ASSERT(0 == IT.size());

            mIT.push_back(source0);
            mIT.push_back(source1);

            mIT.reverse();                                                // 49

            ASSERT(source0 == IT.back());
            ASSERT(source1 == IT.front());

            mIT.sort();                                                   // 50

            ASSERT(source1 == IT.back());
            ASSERT(source0 == IT.front());

            mIT.push_back(source0);

            mIT.sort(comparator);                                         // 51

            ASSERT(source1 == IT.back());

            mIT.splice(IT.begin(), source11);                             // 52

            ASSERT(8 == IT.size());

            mIT.splice(IT.begin(),
                       BloombergLP::bslmf::MovableRefUtil::move(source11));
                                                                          // 53
            ASSERT(8 == IT.size());

            mIT.splice(IT.begin(), source12, source12.cbegin());          // 54

            ASSERT(9 == IT.size());

            mIT.splice(IT.begin(),
                       BloombergLP::bslmf::MovableRefUtil::move(source12),
                       source12.cbegin());                                // 55
            ASSERT(10 == IT.size());

            mIT.splice(IT.begin(),
                       source13,
                       source13.cbegin(),
                       source13.cend());                                  // 56

            ASSERT(15 == IT.size());

            mIT.splice(IT.begin(),
                       BloombergLP::bslmf::MovableRefUtil::move(source13),
                       source13.cbegin(),
                       source13.cend());                                  // 57

            ASSERT(15 == IT.size());

            mIT.unique();                                                 // 58

            ASSERT(2 == IT.size());

            mIT.push_back(source1);

            mIT.unique(equalityPredicate);                                // 59

            ASSERTV(IT.size(), 2 == IT.size());

            mIT.swap(source14);                                           // 60

            ASSERT(initialSize == IT.size()      );
            ASSERT(2           == source14.size());

            ASSERT(IT.end()     != IT.begin()   );                        // 61
            ASSERT(IT.cend()    != IT.cbegin()  );                        // 62
            ASSERT(IT.begin()   != IT.end()     );                        // 63
            ASSERT(IT.cbegin()  != IT.cend()    );                        // 64
            ASSERT(IT.rend()    != IT.rbegin()  );                        // 65
            ASSERT(IT.crend()   != IT.crbegin() );                        // 66
            ASSERT(IT.rbegin()  != IT.rend()    );                        // 67
            ASSERT(IT.crbegin() != IT.crend()   );                        // 68

            ASSERT(false        == IT.empty()   );                        // 69
            ASSERT(0            != IT.max_size());                        // 70
            ASSERT(initialSize  == IT.size()    );                        // 71

            ASSERT(source0 == IT.back() );                                // 72
            ASSERT(source0 == IT.front());                                // 73

            ASSERT(defaultAllocator == IT.get_allocator());               // 74
        }

        // Check if each function has been called.

        IncompleteTypeSupportChecker::checkInvokedFunctions();
      } break;
      case 36: {
        // --------------------------------------------------------------------
        // TESTING FREE FUNCTIONS 'BSL::ERASE' AND 'BSL::ERASE_IF'
        //
        // Concerns:
        //: 1 The free functions exist, and are callable with a list.
        //
        // Plan:
        //: 1 Fill a list with known values, then attempt to erase some of
        //:   the values using 'bsl::erase' and 'bsl::erase_if'.  Verify that
        //:   the resultant list is the right size, contains the correct
        //:   values, and that the value returned from the functions is
        //:   correct.
        //
        // Testing:
        //   size_t erase(list<T,A>&, const U&);
        //   size_t erase_if(list<T,A>&, PREDICATE);
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "\nTESTING FREE FUNCTIONS 'BSL::ERASE' AND 'BSL::ERASE_IF'"
                "\n=======================================================\n");

        TestDriver2<char>::testCase36_erase();
        TestDriver2<int>::testCase36_erase();
        TestDriver2<long>::testCase36_erase();
      } break;
      case 35: {
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
      case 34: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "'noexcept' SPECIFICATION" "\n"
                                 "========================" "\n");

        TestDriver2<int>::testCase34_noexcept();

      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZER LISTS
        //
        // Concerns:
        //: 1 That lists can be constructed from an initializer list.
        //:   o Without an allocator passed.
        //:   o With an allocator passed.
        //:
        //: 2 That lists can be assigned from an initializer list, either
        //:   through
        //:   o 'operator='
        //:   o 'assign'
        //:
        //: 3 That after the operation, the values are correct.
        //:
        //: 4 That if a c'tor throws, no memory is leaked.
        //:
        //: 5 That if an 'operator=' or 'assign' throws, the destination is
        //:   left in a valid state (though it may be changed).
        //:
        //: 6 That the expected amount of memory is allocated.
        //:
        //: 7 If the list is passed a non-default allocator at construction,
        //:   the default allocator is not used.
        //:
        //: 8 'operator=' returns a reference to the modifiable object assigned
        //:   to.
        //
        // Plan:
        //: 1 Whenever possible, do the tests in exception blocks.  (C-4)
        //:   o In the case of assigns (either type), check the integrity of
        //:     the destination early in the block to verify its integrity
        //:     immediately after a throw.  (C-5)
        //:   o in the case of c'tors, make sure any allocator passed, and the
        //:     default allocator, are bslma::TestAllocators, which will detect
        //:     any leaks.  (C-4)
        //:
        //: 2 Create an 'expected' value, do the operation, and compare the
        //:   result to the expected value.  In cases of assignment, make sure
        //:   the initial state of the object is either always (in the case of
        //:   not being done in loops) or usually (in the case of done in
        //:   loops) different from the expected value.  (C-1) (C-2) (C-3)
        //:
        //: 3 Monitor the number of allocations and the blocks in use to verify
        //:   the amount of allocation.  (C-6)
        //:
        //: 4 When the test object is created with a non-default allocator,
        //:   observe that the default allocator is never used.  (C-7)
        //
        // Testing
        //   list(std::initializer_list, const A& = ALLOCATOR());
        //   list& operator=(std::initializer_list);
        //   void assign(std::initializer_list);
        // --------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        if (verbose) printf("TESTING INITIALIZER LISTS\n"
                            "=========================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test33_initializerList,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test33_initializerList,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
#else
        if (verbose) printf("INITIALIZER LISTS NOT TESTED -- CPP11 ONLY\n"
                            "==========================================\n");
#endif
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING SUPPORT FOR RECURSIVE ELEMENT TYPES
        //
        // Concerns:
        //: 1 'list' should support elements of recursive types, that are then
        //:   complete types before operations are instantiated.
        //
        // Plan:
        //: 1 Create a recursive data structure with an object type that can
        //:   hold a 'list' of its own type.  Then manipulate that internal
        //:   list member through public methods of the recursive wrapper type,
        //:   making sure to touch on every method of 'list'.  Note that this
        //:   is mostly a compile-test, and does not need to thoroughly test
        //:   each wrapped function; merely making a single call should satisfy
        //:   the concerns.  (C-1)
        //
        // Testing:
        //   INCOMPLETE VALUE TYPES ARE SUPPORTED
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING SUPPORT FOR RECURSIVE ELEMENT TYPES\n"
                            "===========================================\n");

        Recursive root;
        ASSERT(root.empty());
        ASSERT(0 == root.size());

        root.push_back(root);
        root.push_front(root);
        ASSERT(!root.empty());
        ASSERT(2 == root.size());

        const Recursive copy = root;
        ASSERT(!copy.empty());
        ASSERT(2 == copy.size());

        root.front().pop_back();
        root.back() = copy;
        root.pop_front();

        ASSERT(!root.empty());
        ASSERT(1 == root.size());

        root.pop_back();        // Exercise clearing the list with a member.
        ASSERT(root.empty());   // Note the destructor is tested by 'copy'.
        ASSERT(0 == root.size());
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING MOVE ASSIGN
        //
        // Concerns:
        //: 1 The destination object's value is the same as that of the source
        //:   object (relying on the equality operator) and created with the
        //:   correct allocator.
        //:
        //: 2 If the allocators match, the assignment is a swap.
        //:
        //: 3 If the allocators don't match, the assignment is a copy and the
        //:   value of the 'rhs' is unaffected (this is not a guaranteed
        //:   property of the component -- it's just that for the types we
        //:   test, a 'move' between objects that don't share the same
        //:   allocator is a copy).
        //:
        //: 4 Subsequent changes to the source object have no effect on the
        //:   assigned object.
        //:
        //: 5 The object has its internal memory management system hooked up
        //:   properly so that *all* internally allocated memory draws from a
        //:   user-supplied allocator whenever one is specified.
        //
        // Plan:
        //: 1 Specify a set S of object values with substantial and varied
        //:   differences, ordered by increasing length, to be used in the
        //:   following tests.
        //:
        //: 2 For concerns 1 - 3, for each value in S, initialize objects w and
        //:   v.  Copy object w to x and v to y.  Move assign y to x.
        //:   o If the allocators match, verify it was a swap.
        //:   o If the allocators don't match, verify it was a copy.
        //:
        //: 3 For concern 4, clear x and observe y is unchanged.
        //:
        //: 4 To address concern 5, observe the default allocator before and
        //:   after the whole test and observe that it is never used (after the
        //:   first call to 'checkIntegrity'.
        //
        // Testing:
        //   list& operator=(list&& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING MOVE ASSIGN\n"
                            "===================\n");

#if !defined(BSLS_PLATFORM_CMP_SUN) || BSLS_PLATFORM_CMP_VERSION < 0x5130
        RUN_EACH_TYPE(TestDriver2,
                      test31_moveAssign,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test31_moveAssign,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test31_moveAssign,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        // 'propagate_on_container_move_assignment' testing

        RUN_EACH_TYPE(TestDriver2,
                      test31_propagate_on_container_move_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test31_propagate_on_container_move_assignment,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
        // TBD test 'bsltf::MoveOnlyAllocTestType' here
#else   // Oracle CC 12.4 runs out of memory compiling the full test case
        RUN_EACH_TYPE(TestDriver2,
                      test31_moveAssign,
                      bsltf::MoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test31_moveAssign,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      int);

        // 'propagate_on_container_move_assignment' testing

        RUN_EACH_TYPE(TestDriver2,
                      test31_propagate_on_container_move_assignment,
                      int);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test31_propagate_on_container_move_assignment,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      int);
        // TBD test 'bsltf::MoveOnlyAllocTestType' here
#endif
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTOR
        //
        // Concerns:
        //: 1 The new object's value is the same as the initial value of the
        //:   original object (relying on the equality operator) and created
        //:   with the correct allocator.
        //:
        //: 2 If no allocator is passed, or if the allocators match, the
        //:   operation is a swap.
        //:
        //: 3 if an allocator that does not match the allocator of the source
        //:   object, the operation is a copy (this is not guaranteed by the
        //:   component -- individual elements are moved, but for all the types
        //:   we test, a move with different allocators is a copy.
        //:
        //: 4 Subsequent changes of the source object have no effect on the
        //:   copy-constructed object.
        //:
        //: 5 The object has its internal memory management system hooked up
        //:   properly so that *all* internally allocated memory draws from a
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 6 The function is exception neutral w.r.t. memory allocation.
        //:
        //: 7 An object copied from an rvalue with no allocator, or with a
        //:   matching allocator specified, will leave the copied object in a
        //:   valid, default-constructed state.
        //:   1 No allocator specified to c'tor.
        //:   2 Allocator specified to c'tor.
        //:
        //: 8 An object copied from an rvalue with a non-matching allocator
        //:   supplied will not modify the rvalue.
        //
        // Plan:
        //: 1 Specify a set S of object values with substantial and varied
        //:   differences, ordered by increasing length, to be used in the
        //:   following tests.
        //:
        //: 2 Default-construct a const object D.
        //:
        //: 3 For concerns 1 - 3, for each value in S, initialize objects w and
        //:   x, move construct y from x passing no allocator, and passing
        //:   an allocator that matches 'x's allocator, and in both cases use
        //:   'operator==' to verify that y subsequently has the same value as
        //:   w, and that 'D == x'.
        //:
        //: 4 Modify x and observe y is unchanged.
        //:
        //: 5 Construct an object y from x where the allocators don't match,
        //:   and observe afterward that both y and x equal w.
        //
        // Testing:
        //   list(list&& orig, const A& = A());
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING MOVE CONSTRUCTOR\n"
                            "========================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test30_moveCtor,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test30_moveCtor,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test30_moveCtor,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING MOVE INSERTION
        //
        // Concerns:
        //: 1 That the resulting list value is correct.
        //:
        //: 2 That the 'insert' return (if any) value is a valid iterator to
        //:   the first inserted element or to the insertion position if no
        //:   elements are inserted.
        //:
        //: 3 That insertion of one element has the strong exception guarantee.
        //:
        //: 4 That insertion is exception neutral w.r.t. memory allocation.
        //:
        //: 5 The internal memory management system is hooked up properly so
        //:   that *all* internally allocated memory draws from a user-supplied
        //:   allocator whenever one is specified.
        //:
        //: 6 That inserting a 'const T& value' that is a reference to an
        //:   element of the list does not suffer from aliasing problems.
        //:
        //: 7 That no iterators are invalidated by the insertion.
        //:
        //: 8 That inserting 'n' copies of value 'v' selects the correct
        //:   overload when 'n' and 'v' are identical arithmetic types (i.e.,
        //:   the iterator-range overload is not selected).
        //:
        //: 9 That inserting 'n' copies of value 'v' selects the correct
        //:   overload when 'v' is a pointer type and 'n' is a null pointer
        //:   literal ,'0'.  (i.e., the iterator-range overload is not
        //:   selected).
        //
        // Plan:
        //: 1 Create objects of various sizes and insert a distinct value one
        //:   or more times into each possible position.
        //:
        //: 2 For concerns 1, 2 & 5, verify that the return value and modified
        //:   list are as expected.
        //:
        //: 3 For concerns 3 & 4 perform the test using the exception-testing
        //:   infrastructure and verify the value and memory changes.
        //:
        //: 4 For concern 6, we select the value to insert from the middle of
        //:   the list, thus testing insertion before, at, and after the
        //:   aliased element.
        //:
        //: 5 For concern 7, save copies of the iterators before and after the
        //:   insertion point and verify that they point to the same (valid)
        //:   elements after the insertion by iterating to the same point in
        //:   the resulting list and comparing the new iterators to the old
        //:   ones.
        //:
        //: 6 For concerns 8 and 9, insert 2 elements of integral or pointer
        //:   types into lists and verify that it compiles and that the
        //:   resultant list contains the expected values.
        //
        // Testing:
        //   iterator insert(const_iterator pos, T&& value);
        //   void push_back(T&& value);
        //   void push_front(T&& value);
        //   Concern: All emplace methods handle rvalues.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING MOVE INSERTION\n"
                            "======================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test29_moveInsert,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test29_moveInsert,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test29_moveInsert,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING SORT
        //
        // Concerns:
        //: 1 Sorts correctly in the presence of equivalent (duplicate)
        //:   elements.
        //:
        //: 2 Sorts correctly if the input is already sorted or sorted in
        //:   reverse.
        //:
        //: 3 No memory is allocated or deallocated during the sort.
        //:
        //: 4 No constructors, destructors, or assignment of elements takes
        //:   place.
        //:
        //: 5 Iterators to all elements remain valid.
        //:
        //: 6 The predicate version of 'sort' can be used to sort using a
        //:   different comparison criterion.
        //:
        //: 7 The non-predicate version of 'sort' does not use 'std::less'.
        //:
        //: 8 The sort is stable -- equivalent elements remain in the same
        //:   order as in the original list.
        //:
        //: 9 The number of calls to the comparison operation is no larger than
        //:   'N*log2(N)', where 'N' is the number of elements.
        //:
        //: 10 If the comparison function throws an exception, no memory is
        //:    leaked.  (The order of the elements is indeterminate.)
        //
        // Plan:
        //: 1 Create a series of list specifications of different lengths, some
        //:   containing duplicates, triplicates, and multiple sets of
        //:   duplicates and triplicates.
        //:
        //: 2 Generate every permutation of elements within each specification.
        //:
        //: 3 Create a list from the permutation, store an iterator to each
        //:   list element, and sort the list.
        //:
        //: 4 Verify that:
        //:   o The resultant list is a sorted version of the original.
        //:   o Iterating over each element in the sorted list results in an
        //:     iterator that existed in the original list.
        //:   o For equivalent elements, the iterators appear in the same
        //:     order.
        //:
        //: 5 Test allocations, constructor counts, destructor counts, and
        //:   assignment counts before and after the sort and verify that they
        //:   haven't changed.  (Constructor, destructor, and assignment counts
        //:   are meaningful only if 'TYPE' is 'TestType', but will are
        //:   accessible and will remain unchanged anyway for other types.)
        //:
        //: 6 To address concern 7, std::less<TestType> is specialized to
        //:   detect being called inappropriately.
        //:
        //: 7 To address concern 6, repeat the test using a predicate that
        //:   sorts in reverse order.
        //:
        //: 8 To address concern 9, the predicate counts the number of
        //:   invocations.
        //:
        //: 9 To address concern 10, the predicate operator is instrumented to
        //:   throw an exception after a specific number of iterations.
        //:
        //: 10 Using a sample string, set the comparison operator to throw at
        //:    different counts and verify, after each exception, that:
        //:    o No memory is leaked.
        //:    o The list is valid.
        //:    o Every element in the list is represented by a saved iterator.
        //
        // Testing:
        //   void sort();
        //   template <class COMP> void sort(COMP c);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING SORT\n"
                            "============\n");

        if (verbose) printf("... with 'char'.\n");
        TestDriver2<char>::test28_sort();

        if (verbose) printf("... with 'TestType'.\n");
        TestDriver2<TTA>::test28_sort();
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING MERGE
        //
        // Concerns:
        //: 1 Merging produces correct results with and without duplicate
        //:   elements within and between the lists to be merged.
        //:
        //: 2 The argument to merge is empty after the merge.
        //:
        //: 3 No memory is allocated or deallocated during the merge.
        //:
        //: 4 No constructors, destructors, or assignment of elements takes
        //:   place.
        //:
        //: 5 Iterators to all elements remain valid.
        //:
        //: 6 The predicate version of 'merge' can be used to merge using a
        //:   different comparison criterion.
        //:
        //: 7 The non-predicate version of 'merge' does not use 'std::less'.
        //:
        //: 8 Merging a list with itself has no effect.
        //:
        //: 9 If the comparison function throws an exception, no memory is
        //:   leaked and all elements remain in one list or the other.
        //
        // Plan:
        //: 1 Create two lists from the cross-product of two small sets of
        //:   specifications.  The elements in the lists are chosen so that
        //:   every combination of duplicate and non-duplicate elements, both
        //:   within and between lists, is represented.
        //:
        //: 2 Save the iterators to all elements of both lists and record the
        //:   memory usage before the merge.
        //:
        //: 3 Merge one list into the other.
        //:
        //: 4 Verify that:
        //:   o The merged value is correct.
        //:   o All of the pre-merge iterators are still valid.
        //:   o The non-merged list is now empty.
        //:
        //: 5 To address concern 6, sort the initial specifications using the
        //:   reverse sort order, then use a custom "greater-than" predicate to
        //:   merge the lists and verify the same values as for the
        //:   non-predicate case.
        //:
        //: 6 To address concern 7, std::less<TestType> is specialized to
        //:   detect being called inappropriately.
        //:
        //: 7 To address concern 8, merge each list with itself and verify that
        //:   no memory is allocated or deallocated and that all iterators
        //:   remain valid.
        //
        // Testing:
        //   void merge(list& other);
        //   void merge(list&& other);
        //   template <class COMP> void merge(list& other, COMP c);
        //   template <class COMP> void merge(list&& other, COMP c);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING MERGE\n"
                            "=============\n");

        RUN_EACH_TYPE(TestDriver2,
                      test27_merge,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test27_merge,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test27_merge,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING UNIQUE
        //
        // Concerns:
        //: 1 The predicate and non-predicate versions of 'unique' have
        //:   essentially the same characteristics.
        //:
        //: 2 Can remove elements from any or all positions in the list except
        //:   the first.
        //:
        //: 3 Destructors are called for removed elements and memory is deleted
        //:   for removed elements.
        //:
        //: 4 No constructors, destructors, or assignment operators are called
        //:   on the remaining (non-removed) elements.
        //:
        //: 5 No memory is allocated.
        //:
        //: 6 Iterators to non-removed elements, including the 'end()'
        //:   iterator, remain valid after removal.
        //:
        //: 7 The non-removed elements retain their relative order.
        //:
        //: 8 The 'unique' operation is exception-neutral, if the equality
        //:   operator or predicate throw an exception.
        //:
        //: 9 The non-predicate version calls operator==(T,T) directly; it does
        //:   not call std::equal_to<T>::operator()(T,T).
        //
        // Plan:
        //: 1 For concern 1, perform the same tests for both the predicate and
        //:   non-predicate versions of 'unique.
        //:
        //: 2 Generate lists of various lengths up to 10 elements, filling the
        //:   lists with different sequences of values such that every
        //:   combination of matching and non-matching subsequences is
        //:   generated.  (For the predicate version, matching elements need to
        //:   be equal only in their low bit).
        //:
        //: 3 For each combination, make a copy of all of the iterators to
        //:   non-repeated elements, then call 'unique'.
        //:
        //: 4 Validate that:
        //:   o The number of new destructor calls matches the number of
        //:     elements removed.
        //:   o Reduction of memory blocks in use is correct for the number
        //:     elements removed.
        //:   o The number of new allocations is zero, the number of new
        //:     constructor calls is zero.
        //:   o The iterating over the remaining elements produces a sequence
        //:     of values and iterators matching those saved before the
        //:     'unique' operation.
        //:
        //: 5 For concern 8, perform the tests in an exception-testing
        //:   framework, using a special feature of the 'LowBitEQ' predicate to
        //:   cause exceptions to be thrown.
        //:
        //: 6 For concern 9, std::equal_to<TestType> is specialized to detect
        //:   being called inappropriately.
        //
        // Testing:
        //   void unique();
        //   template <class BINPRED> void unique(BINPRED p);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING UNIQUE\n"
                            "==============\n");


        RUN_EACH_TYPE(TestDriver2,
                      test26_unique,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test26_unique,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test26_unique,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING REMOVE
        //
        // Concerns:
        //: 1 'remove' and 'remove_if' have essentially the same
        //:   characteristics.
        //:
        //: 2 Will remove 0..N elements from an N-element list.
        //:
        //: 3 Can remove elements from any or all positions in the list
        //:
        //: 4 Destructors are called for removed elements and memory is deleted
        //:   for removed elements.
        //:
        //: 5 No constructors, destructors, or assignment operators are called
        //:   on the remaining (non-removed) elements.
        //:
        //: 6 No memory is allocated.
        //:
        //: 7 Iterators to non-removed elements, including the 'end()'
        //:   iterator, remain valid after removal.
        //:
        //: 8 The non-'E' elements retain their relative order.
        //
        // Plan:
        //: 1 For concern 1, perform the same tests for both 'remove' and
        //:   'remove_if'.
        //:
        //: 2 Generate lists from a small set of specifications from empty to
        //:   10 elements, none of which contain the value 'E'.
        //:
        //: 3 Replace 0 to 'LENGTH' elements with the value 'E', in every
        //:   possible combination.
        //:
        //: 4 For each specification and combination, make a copy of all of the
        //:   iterators to non-'E' elements, then call 'remove' or 'remove_if'.
        //:
        //: 5 Validate that:
        //:   o The number of new destructor call matches the number of
        //:     elements removed.
        //:   o Reduction of memory blocks in use is correct for the number
        //:     elements removed.
        //:   o The number of new allocations is zero.
        //:   o The number of new constructor calls is zero.
        //:   o The iterating over the remaining elements produces a sequence
        //:     of values and iterators matching those saved before the remove
        //:     operation.
        //
        // Testing:
        //   void remove(const T& val);
        //   template <class PRED> void remove_if(PRED p);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING REMOVE\n"
                            "==============\n");

        RUN_EACH_TYPE(TestDriver2,
                      test25_remove,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test25_remove,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test25_remove,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING SPLICE
        //
        // Concerns:
        //: 1 Can splice into any position within target list.
        //:
        //: 2 Can splice from any position within source list.
        //:
        //: 3 No iterators or pointers are invalidated.
        //:
        //: 4 No allocations or deallocations occur.
        //:
        //: 5 No constructor calls, destructor calls, or assignments occur.
        //
        // Plan:
        //: 1 Perform a small area test with source and target lists of 0 to 5
        //:   elements each, splicing into every target position from every
        //:   source position and every source length.
        //:
        //: 2 Keep track of the original iterators and element addresses from
        //:   each list and verify that they remain valid and point to the
        //:   correct element in the post-splice lists.
        //:
        //: 3 Query the number of allocations, deallocations, constructor
        //:   calls, destructor calls, and assignment operator calls before and
        //:   after each splice and verify that they do not change.
        //
        // Testing:
        //   void splice(iterator pos, list& other);
        //   void splice(iterator pos, list&& other);
        //   void splice(iterator pos, list& other, iterator i);
        //   void splice(iterator pos, list&& other, iterator i);
        //   void splice(iter pos, list& other, iter first, iter last);
        //   void splice(iter pos, list&& other, iter first, iter last);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING SPLICE\n"
                            "==============\n");

        RUN_EACH_TYPE(TestDriver2,
                      test24_splice,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test24_splice,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING REVERSE
        //
        // Concerns:
        //: 1 Reversing a list produced the correct result with 0, 1, 2, or
        //:   more elements.
        //:
        //: 2 Reversing a list with duplicate elements works as expected.
        //:
        //: 3 No constructors, destructors, or assignment operators of
        //:   contained elements are called.
        //:
        //: 4 No memory is allocated or deallocated.
        //
        // Plan:
        //: 1 Create a list from a variety of specifications, including empty
        //:   lists, lists of different lengths, and lists with consecutive or
        //:   non-consecutive duplicate elements and call 'reverse' on the
        //:   list.
        //:
        //: 2 For concerns 1 and 2, verify that calling 'reverse' produces the
        //:   expected result.
        //:
        //: 3 For concern 3, compare the counts of 'TestType' constructors and
        //:   destructors before and after calling 'reverse' and verify that
        //:   they do not change.
        //:
        //: 4 For concern 4, use a test allocator and compare the counts of
        //:   total blocks allocated and blocks in use before and after calling
        //:   'reverse' and verify that the counts do not change.
        //
        // Testing:
        //   void reverse();
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING REVERSE\n"
                            "===============\n");

        RUN_EACH_TYPE(TestDriver2,
                      test23_reverse,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test23_reverse,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test23_reverse,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        //
        // Concerns:
        //: 1 That the list has the 'bslalg::HasStlIterators' trait.
        //:
        //: 2 If instantiated with 'bsl::allocator', then list has the
        //:   'bslma::UsesBslmaAllocator' trait.
        //:
        //: 3 If instantiated with an allocator that is bitwise movable, then
        //:   the list has the 'bslmf::IsBitwiseMoveable' trait.
        //:
        //: 4 That a list never has the 'bsl::is_trivially_copyable' trait.
        //
        // Plan:
        //: 1 Test each of the above four traits and compare their values to
        //:   the expected values.
        //
        // Testing:
        //   bslalg::HasStlIterators
        //   bslma::UsesBslmaAllocator
        //   bslmf::IsBitwiseMoveable
        //   !bsl::is_trivially_copyable
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING TYPE TRAITS\n"
                            "===================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test22_typeTraits,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test22_typeTraits,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING TYPEDEFS
        //
        // Concerns:
        //: 1 That all of the required typedefs are defined.
        //:
        //: 2 That the typedefs are identical to the corresponding typedefs
        //:   from the allocator.
        //
        // Plan:
        //: 1 Use compile-time asserts calling the 'bsl::is_same' template to
        //:   test that various types match.  Note that the iterator types were
        //:   tested in test case 16 and so are not tested here.
        //
        // Testing:
        //   type reference
        //   type const_reference
        //   type size_type
        //   type difference_type
        //   type value_type
        //   type allocator_type
        //   type pointer
        //   type const_pointer
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING TYPEDEFS\n"
                            "================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test21_typedefs,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test21_typedefs,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON FREE OPERATORS
        //
        // Concerns:
        //: 1 'operator<' returns the lexicographic comparison on two lists.
        //:
        //: 2 'operator>', 'operator<=', and 'operator>=' are correctly tied to
        //:   'operator<'.
        //:
        //: 3 'operator<=>' is consistent with '<', '>', '<=', '>='.
        //:
        //: 4 That traits get selected properly.
        //
        // Plan:
        //: 1 For a variety of lists of different sizes and different values,
        //:   test that the comparison returns as expected.
        //
        // Testing:
        //   bool operator<(const list&, const list&);
        //   bool operator>(const list&, const list&);
        //   bool operator<=(const list&, const list&);
        //   bool operator>=(const list&, const list&);
        //   bool operator>=(const list&, const list&);
        //   auto operator<=>(const list&, const list&);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING COMPARISON FREE OPERATORS\n"
                            "=================================\n");

        // Comparison operators only work for types that have 'operator<'
        // defined.

        RUN_EACH_TYPE(TestDriver2,
                      test20_comparisonOps,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test20_comparisonOps,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING SWAP
        //
        // Concerns:
        //: 1 Swapping containers does not swap allocators.
        //:
        //: 2 Swapping containers with same allocator results in no allocation
        //:   or deallocation operations.
        //:
        //: 3 Swapping containers with the same allocator causes iterators to
        //:   remain valid but to refer to the opposite container.
        //:
        //: 4 DEPRECATED: Swapping containers with different allocator
        //:   instances will have the same memory usage copy-constructing each
        //:   container and destroying the original.
        //:
        //: 5 DEPRECATED: An exception thrown while swapping containers with
        //:   different allocator instances will leave the containers
        //:   unchanged.
        //
        // Plan:
        //: 1 Construct 'lst1' and 'lst2' with same test allocator.
        //:
        //: 2 Add data to each list.  Remember allocation statistics and
        //:   iterators.
        //:
        //: 3 Verify that contents were swapped.
        //:
        //: 4 Verify that allocator is unchanged.
        //:
        //: 5 Verify that no memory was allocated or deallocated.
        //:
        //: 6 Verify that each iterator now refers to the same element in the
        //:   other container.
        //:
        //: 7 For concerns 4 and 5, construct two containers with different
        //:   allocators and swap them within an exception test harness.
        //:   Verify the expected memory usage and verify that an exception
        //:   leaves the containers unchanged.
        //
        // Testing:
        //   void swap(Obj& rhs);
        //   void bsl::swap(Obj& lhs, Obj& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING SWAP\n"
                            "============\n");

        RUN_EACH_TYPE(TestDriver2,
                      test19_swap,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test19_swap,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test19_swap,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        // 'propagate_on_container_swap' testing

        RUN_EACH_TYPE(TestDriver2,
                      test19_propagate_on_container_swap,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // TBD test 'bsltf::MoveOnlyAllocTestType' here
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING ERASE AND POP
        //
        // Concerns:
        //: 1 That the resulting value is correct.
        //:
        //: 2 That erase operations do not allocate memory.
        //:
        //: 3 That no memory is leaked.
        //
        // Plan:
        //: 1 For the 'erase' methods, the concerns are simply to cover the
        //:   full range of possible indices and numbers of elements.  We build
        //:   a list with a variable size and capacity, and remove a variable
        //:   element or number of elements from it, and verify that size,
        //:   capacity, and value are as expected:
        //:   o Without exceptions, and computing the number of allocations.
        //:   o That the total allocations do not increase.
        //:   o That the in-use allocations diminish by the correct amount.
        //
        // Testing:
        //   void pop_back();
        //   void pop_front();
        //   iterator erase(const_iterator pos);
        //   iterator erase(const_iterator first, const_iterator last);
        // -------------------------------------------------------------------

        if (verbose) printf("TESTING ERASE AND POP\n"
                            "=====================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test18_erase,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test18_erase,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test18_erase,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING INSERTION
        //
        // Concerns
        //: 1 For single-element insert/push, or insertion of copies of the
        //:   same element:
        //:
        //:   1 That the resulting list value is correct.
        //:
        //:   2 That the 'insert' return (if any) value is a valid iterator to
        //:     the first inserted element or to the insertion position if no
        //:     elements are inserted.
        //:
        //:   3 That insertion of one element has the strong exception
        //:     guarantee.
        //:
        //:   4 That insertion is exception neutral w.r.t. memory allocation.
        //:
        //:   5 The internal memory management system is hooked up properly so
        //:     that *all* internally allocated memory draws from a
        //:     user-supplied allocator whenever one is specified.
        //:
        //:   6 That inserting a 'const T& value' that is a reference to an
        //:     element of the list does not suffer from aliasing problems.
        //:
        //:   7 That no iterators are invalidated by the insertion.
        //:
        //:   8 That inserting 'n' copies of value 'v' selects the correct
        //:     overload when 'n' and 'v' are identical arithmetic types (i.e.,
        //:     the iterator-range overload is not selected).
        //:
        //:   9 That inserting 'n' copies of value 'v' selects the correct
        //:     overload when 'v' is a pointer type and 'n' is a null pointer
        //:     literal ,'0'.  (i.e., the iterator-range overload is not
        //:     selected).
        //:
        //: 2 For 'emplace':
        //:
        //:   1 That the resulting list value is correct.
        //:
        //:   2 That the 'emplace' return (if any) value is a valid iterator to
        //:     the first inserted element or to the insertion position if no
        //:     elements are inserted.
        //:
        //:   3 That 'emplace' has the strong exception guarantee.
        //:
        //:   4 That 'emplace' is exception neutral w.r.t. memory allocation.
        //:
        //:   5 The internal memory management system is hooked up properly so
        //:     that *all* internally allocated memory draws from a
        //:     user-supplied allocator whenever one is specified.
        //:
        //:   6 That inserting a 'const T& value' that is a reference to an
        //:     element of the list does not suffer from aliasing problems.
        //:
        //:   7 That no iterators are invalidated by the insertion.
        //:
        //:   8 That 'emplace' passes 0 to 5 arguments to the 'T' constructor.
        //:
        //: 3 For range insertion:
        //:
        //:   1 That the resulting list value is correct.
        //:
        //:   2 That the 'insert' return (if any) value is a valid iterator to
        //:     the first inserted element or to the insertion position if no
        //:     elements are inserted.
        //:
        //:   3 That insertion of one element has the strong exception
        //:     guarantee.
        //:
        //:   4 That insertion is exception neutral w.r.t. memory allocation.
        //:
        //:   5 The internal memory management system is hooked up properly so
        //:     that *all* internally allocated memory draws from a
        //:     user-supplied allocator whenever one is specified.
        //:
        //:   6 That no iterators are invalidated by the insertion.
        //
        // Plan:
        //: 1 For single-element insert/push, or insertion of copies of the
        //:   same element:
        //:
        //:   1 Create objects of various sizes and insert a distinct value one
        //:     or more times into each possible position.
        //:
        //:     o For concerns 1, 2 & 5, verify that the return value and
        //:       modified list are as expected.
        //:
        //:     o For concerns
        //:       3 & 4 perform the test using the exception-testing
        //:         infrastructure and verify the value and memory changes.
        //:
        //:     o For concern 6, we select the value to insert from the middle
        //:       of the list, thus testing insertion before, at, and after the
        //:       aliased element.
        //:
        //:     o For concern 7, save copies of the iterators before and after
        //:       the insertion point and verify that they point to the same
        //:       (valid) elements after the insertion by iterating to the same
        //:       point in the resulting list and comparing the new iterators
        //:       to the old ones.
        //:
        //:     o For concerns 8 and 9, insert 2 elements of integral or
        //:       pointer types into lists and verify that it compiles and that
        //:       the resultant list contains the expected values.
        //:
        //: 2 For 'emplace':
        //:
        //:   1 Create objects of various sizes and insert a distinct value
        //:     into each possible position.
        //:
        //:   2 For concerns 1, 2 & 5, verify that the return value and
        //:     modified list are as expected.
        //:
        //:   3 For concerns 3 & 4 perform the test using the exception-testing
        //:     infrastructure and verify the value and memory changes.
        //:
        //:   4 For concern 6, we select the value to insert from the middle of
        //:     the list, thus testing insertion before, at, and after the
        //:     aliased element.
        //:
        //:   5 For concern 7, save copies of the iterators before and after
        //:     the insertion point and verify that they point to the same
        //:     (valid) elements after the insertion by iterating to the same
        //:     point in the resulting list and comparing the new iterators to
        //:     the old ones.
        //:
        //:   6 For concern 8, test each 'emplace' call with 0 to 5 arguments.
        //:     The test types are designed to ignore all but the last
        //:     argument, but verify that the preceding arguments are the
        //:     values '1', '2', '3', and '4'.
        //:
        //: 3 For range insertion:
        //:
        //:   1 Create objects of various sizes and insert a range of 0 to 3
        //:     values at each possible position.
        //:
        //:   2 For concerns 1, 2 & 5, verify that the return value and
        //:     modified list are as expected.
        //:
        //:   3 For concerns 3 & 4 perform the test using the exception-testing
        //:     infrastructure and verify the value and memory changes.
        //:
        //:   4 For concern 7, save copies of the iterators before and after
        //:     the insertion point and verify that they point to the same
        //:     (valid) elements after the insertion by iterating to the same
        //:     point in the resulting list and comparing the new iterators to
        //:     the old ones.
        //
        // Testing:
        //   iterator insert(const_iterator pos, const T& value);
        //   iterator insert(const_iterator pos, size_type n, const T& value);
        //   void push_back(const T& value);
        //   void push_front(const T& value);
        //   iterator emplace(const_iterator pos, Args&&... args);
        //   reference emplace_back(Args&&... args);
        //   reference emplace_front(Args&&... args);
        //   template <class Iter> iterator insert(CIter pos, Iter f, Iter l);
        // -------------------------------------------------------------------

        if (verbose) printf("TESTING INSERTION\n"
                            "=================\n");

        if (verbose) printf("Testing Value Insertion\n"
                            "=======================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test17_insert,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test17_insert,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        if (verbose) printf("Testing Emplace Insertion\n"
                            "=========================\n");

        // 'TestEmplace' cannot work on any type other than 'TestType', since
        // it requires the class to have a c'tor with 4 integer args.

        RUN_EACH_TYPE(TestDriver2,
                      test17_emplace,
                      TTA);

        // 'bsltf::AllocEmplacableTestType' and 'bsltf::EmplacableTestType'
        // don't work here, because they don't have c'tors that take a variable
        // number of 'int' args.

        if (verbose) printf("... with 'TestType'.\n");
        TestDriver2<TTA>::test17_emplace();

        if (verbose) printf("Testing Range Insertion\n"
                            "=======================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test17_insertRange,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test17_insertRange,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        if (verbose) printf("Testing overloading disambiguation.\n");
        {
            // 'n' and 'v' are identical arithmetic types.  Make sure overload
            // resolution doesn't try to call the iterator-range 'insert'.

            {
                bsl::list<size_t>  x;
                bsl::list<size_t>& X = x;
                size_t             n = 2, v = 99;

                x.insert(X.begin(), n, v);
                ASSERT(X.size()  == n);
                ASSERT(X.front() == v);
                ASSERT(X.back()  == v);
            }
            {
                bsl::list<IntWrapper>  x;
                bsl::list<IntWrapper>& X = x;
                unsigned char     n = 2, v = 99;

                x.insert(X.begin(), n, v);
                ASSERT(X.size()  == n);
                ASSERT(X.front() == v);
                ASSERT(X.back()  == v);
            }
            {
                bsl::list<IntWrapper>  x;
                bsl::list<IntWrapper>& X = x;
                size_t            n = 2;
                int               v = 99;

                x.insert(X.begin(), n, v);
                ASSERT(X.size()  == n);
                ASSERT(X.front() == v);
                ASSERT(X.back()  == v);
            }
            {
                // TBD: the below code block causes warnings.
                //bsl::list<IntWrapper, ALLOC> x;
                //bsl::list<IntWrapper, ALLOC>& X = x;
                //float n = 2, v = 99;

                //x.insert(X.begin(), n, v);
                //ASSERT(X.size()  == n);
                //ASSERT(X.front() == v);
                //ASSERT(X.back()  == v);
            }

            {
                bsl::list<IntWrapper>  x;
                bsl::list<IntWrapper>& X = x;
                TestEnum          n = TWO, v = NINETYNINE;

                x.insert(X.begin(), n, v);
                ASSERT(X.size()  == (size_t)n);
                ASSERT(X.front() == v);
                ASSERT(X.back()  == v);
            }

            // 'n' is an 'int' and 'v' is a zero 'int' literal (which is also a
            // null pointer literal).  Make sure that it is correctly treated
            // as a pointer.

            {
                bsl::list<char*>   x;
                bsl::list<char*>&  X = x;
                int           n = 2;
                char         *v = 0;

                x.insert(X.begin(), n, 0);  // Literal null, acts like an int.
                ASSERT(X.size()  == (size_t)n);
                ASSERT(X.front() == v);
                ASSERT(X.back()  == v);
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        //
        // Concerns:
        //: 1 That 'iterator' and 'const_iterator' are bi-directional
        //:   iterators.
        //:
        //: 2 That 'iterator' and 'const_iterator' are CopyConstructible,
        //:   Assignable, and EqualityComparable, that 'iterator' is
        //:   convertible to 'const_iterator', and that 'reverse_iterator' is
        //:   constructible from 'iterator'.
        //:
        //: 3 That 'begin' and 'end' return mutable iterators for a reference
        //:   to a modifiable list, and non-mutable iterators otherwise.
        //:
        //: 4 That the iterators can be dereferenced using 'operator*' or
        //:   'operator->', yielding a reference or pointer with the correct
        //:   constness.
        //:
        //: 5 That the range '[begin(), end())' equals the value of the list.
        //:
        //: 6 That iterators can be pre-incremented, post-incremented,
        //:   pre-decremented, and post-decremented.
        //:
        //: 7 Same concerns with 'rbegin', 'rend', 'reverse_iterator', and
        //:   'const_reverse_iterator'.
        //
        // Plan:
        //: 1 For concerns 1, 3, 4, and 7 create a one-element list and verify
        //:   the static properties of 'iterator', 'const_iterator',
        //:   ''reverse_iterator', and 'const_reverse_iterator'.
        //:
        //: 2 For concerns 1, 2, 5, 6, and 7, for each value given by variety
        //:   of specifications of different lengths, create a test list with
        //:   this value, and access each element in sequence and in reverse
        //:   sequence, both as a reference providing modifiable access
        //:   (setting it to a default value, then back to its original value),
        //:   and as a reference providing non-modifiable access.  At each step
        //:   in the traversal, save the current iterator using both copy
        //:   construction and assignment and, in a nested second loop,
        //:   traverse the whole list in reverse order, testing that the
        //:   nested-traversal iterator matches the saved iterator if they
        //:   refer to the same element.
        //
        // Testing:
        //   type iterator
        //   type reverse_iterator
        //   type const_iterator
        //   type const_reverse_iterator
        //   iterator begin();
        //   iterator end();
        //   reverse_iterator rbegin();
        //   reverse_iterator rend();
        //   const_iterator begin() const;
        //   const_iterator end() const;
        //   const_reverse_iterator rbegin() const;
        //   const_reverse_iterator rend() const;
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING ITERATORS\n"
                            "=================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test16_iterators,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test16_iterators,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test16_iterators,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING ELEMENT ACCESS
        //
        // Concerns:
        //: 1 That 'v.front()' and 'v.back()', allow modifying the element when
        //:   'v' is modifiable, but must not modify the element when it is
        //:   'const'.
        //
        // Plan:
        //: 1 For each value given by variety of specifications of different
        //:   lengths, create a test list with this value, and access the first
        //:   and last elements (front, back) both as a reference providing
        //:   modifiable access (setting it to a default value, then back to
        //:   its original value), and as a reference providing non-modifiable
        //:   access.
        //
        // Testing:
        //   reference front();
        //   reference back();
        //   const_reference front() const;
        //   const_reference back() const;
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING ELEMENT ACCESS\n"
                            "======================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test15_elementAccess,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test15_elementAccess,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test15_elementAccess,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING MAX_SIZE AND RESIZE
        //
        // Concerns:
        //: 1 For 'max_size':
        //:
        //:   1 The reported value is no more than one less than the maximum
        //:     allowed by the allocator.
        //:
        //:   2 The allocator's 'max_size' is honored.
        //:
        //: 2 For 'resize':
        //:
        //:   1 Resized list has the correct value.
        //:
        //:   2 Resizing to the current size allocates and frees no memory.
        //:
        //:   3 Resizing to a smaller size allocates no memory.
        //:
        //:   4 Resizing to a larger size frees no memory.
        //:
        //:   5 Resizing to a larger size propagates the allocator to elements
        //:     appropriately.
        //:
        //:   6 'resize' is exception neutral.
        //
        // Plan:
        //: 1 For 'max_size':
        //:
        //:   1 Using the default allocator, test that 'max_size' returns a
        //:     value no larger than all of memory divided by the size of one
        //:     element.
        //:
        //:   2 Repeat this test with 'char' and TestType' element types.
        //:     Using the 'LimitAllocator', test that 'max_size' returns the
        //:     same value as 'LimitAllocator<T>::max_size()', except that a
        //:     node of overhead is allowed to be subtracted from the result.
        //:
        //: 2 For 'resize':
        //:
        //:   1 Using a set of input specs and result sizes, try each
        //:     combination with and without specifying a value for the new
        //:     elements.  Verify each of the above concerns for each
        //:     combination.
        //
        // Testing:
        //   size_type max_size() const;
        //   void resize(size_type n);
        //   void resize(size_type n, const T& val);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING MAX_SIZE AND RESIZE\n"
                            "===========================\n");

        if (verbose) printf("Testing 'max_size'\n"
                            "==================\n");

        if (verbose) printf("... with 'char'.\n");
        {
            bsl::list<char> X;
            ASSERT(~(size_t)0 / sizeof(char) >= X.max_size());
        }

        if (verbose) printf("... with 'TestType'.\n");
        {
            bsl::list<TestType> X;
            ASSERT(~(size_t)0 / sizeof(TestType) >= X.max_size());
        }

        if (verbose) printf("... with 'int' and 'LimitAllocator.\n");
        {
            typedef LimitAllocator<bsl::allocator<int> > LimA;

            const int LIMIT = 10;
            LimA      a;
            a.setMaxSize(LIMIT);

            bsl::list<int,LimA> X(a);

            // LimitAllocator will return the same 'max_size' regardless of the
            // type on which it is instantiated.  Thus, it will report that it
            // can allocate the same number of nodes as 'int's.  (This behavior
            // is not typical for an allocator, but works for this test.)  The
            // 'list' should have no more than one node of overhead.

            ASSERT(LIMIT     >= (int) X.max_size());
            ASSERT(LIMIT - 1 <= (int) X.max_size());
        }

        if (verbose) printf("Testing 'resize'.\n");

        RUN_EACH_TYPE(TestDriver2,
                      test14_resize,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test14_resize,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test14_resize,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING INITIAL-LENGTH AND RANGE ASSIGNMENT
        //
        // Concerns:
        //: 1 Initial-length assignment:
        //:
        //:   1 The assigned value is correct.
        //:
        //:   2 The 'assign' call is exception neutral w.r.t. memory
        //:     allocation.
        //:
        //:   3 The internal memory management system is hooked up properly so
        //:     that *all* internally allocated memory draws from a
        //:     user-supplied allocator whenever one is specified.
        //:
        //: 2 Range assignment:
        //:
        //:   1 That the initial value is correct.
        //:
        //:   2 That the initial range is correctly imported if the initial
        //:     'FWD_ITER' is an input iterator.
        //:
        //:   3 That the constructor is exception neutral w.r.t. memory
        //:     allocation.
        //:
        //:   4 That the internal memory management system is hooked up
        //:     properly so that *all* internally allocated memory draws from a
        //:     user-supplied allocator whenever one is specified.
        //:
        //:   5 The previous value is freed properly.
        //
        // Plan:
        //: 1 Initial-length assignment:
        //:
        //:   1 For the assignment we will create objects of varying sizes
        //:     containing default values for type T, and then assign different
        //:     'value'.  Perform the above tests:
        //:
        //:     o With various initial values before the assignment.
        //:
        //:     o In the presence of exceptions during memory allocations using
        //:       a 'bslma_TestAllocator' and varying its *allocation* *limit*.
        //:
        //:   2 Use basic accessors and equality comparison to verify that
        //:     assignment was successful.
        //:
        //: 2 Range assignment:
        //:
        //:   1 For the assignment we will create objects of varying sizes
        //:     containing default values for type T, and then assign different
        //:     'value' as argument.  Perform the above tests:
        //:
        //:     o From the parameterized 'CONTAINER::const_iterator'.
        //:
        //:     o In the presence of exceptions during memory allocations using
        //:       a 'bslma_TestAllocator' and varying its *allocation* *limit*.
        //:
        //:   2 Use basic accessors to verify:
        //:
        //:     o size
        //:
        //:     o capacity
        //:
        //:     o element value at each index position { 0 .. length - 1 }.
        //
        // Note that we relax the concerns about memory consumption, since this
        // is implemented as 'erase + insert', and insert will be tested more
        // completely in test case 17.
        //
        // Testing:
        //   void assign(size_type numElements, const T& val);
        //   template <class Iter> void assign(Iter first, Iter last);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING INITIAL-LENGTH AND RANGE ASSIGNMENT\n"
                            "===========================================\n");

        if (verbose) printf("TESTING INITIAL-LENGTH ASSIGNMENT\n"
                            "=================================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test13_initialLengthAssign,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test13_initialLengthAssign,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        if (verbose) printf("TESTING RANGE ASSIGNMENT\n"
                            "========================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test13_assignRange,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test13_assignRange,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING INITIAL-LENGTH, RANGE CONSTRUCTORS
        //
        // Concerns:
        //: 1 Initial-length constructor:
        //:
        //:   1 The initial value is correct.
        //:
        //:   2 The constructor is exception neutral w.r.t. memory allocation.
        //:
        //:   3 The internal memory management system is hooked up properly so
        //:     that *all* internally allocated memory draws from a
        //:     user-supplied allocator whenever one is specified.
        //:
        //:   4 TBD: The C++0x move constructor moves value and allocator
        //:     correctly, and without performing any allocation.
        //:
        //:   5 Constructing a list with 'n' copies of value 'v' selects the
        //:     correct overload when 'n' and 'v' are identical arithmetic
        //:     types (i.e., the iterator-range overload is not selected).
        //:
        //:   6 Constructing a list with 'n' copies of value 'v' selects the
        //:     correct overload when 'v' is a pointer type and 'n' is a null
        //:     pointer literal ,'0'.  (i.e., the iterator-range overload is
        //:     not selected).
        //:
        //: 2 Range constructor (*NOT* the C++11 'std::initializer_list'
        //:   c'tor):
        //:
        //:   1 That the initial value is correct.
        //:
        //:   2 That the initial range is correctly imported if the initial
        //:     'FWD_ITER' is an input iterator.
        //:
        //:   3 That the constructor is exception neutral w.r.t. memory
        //:     allocation.
        //:
        //:   4 That the internal memory management system is hooked up
        //:     properly so that *all* internally allocated memory draws from a
        //:     user-supplied allocator whenever one is specified.
        //
        // Plan:
        //: 1 Initial-length constructor:
        //:
        //:   1 For the constructor we will create objects of varying sizes
        //:     with different 'value' as argument.  Test first with the
        //:     default value for type T, and then test with different values.
        //:     Perform the above tests:
        //:
        //:     o With and without passing in an allocator.
        //:
        //:     o In the presence of exceptions during memory allocations using
        //:       a 'bslma_TestAllocator' and varying its *allocation* *limit*.
        //:
        //:     o Where the object is constructed with an object allocator, and
        //:       neither of global and default allocator is used to supply
        //:       memory.
        //:
        //:   2 Use basic accessors to verify
        //:
        //:     o size
        //:
        //:     o allocator
        //:
        //:     o element value at each iterator position { begin() .. end() }.
        //:
        //:   3 As for concern 4, we simply move-construct each value into a
        //:     new list and check that the value, and allocator are as
        //:     expected, and that no allocation was performed.
        //:
        //:   4 For concerns 5 and 6, construct a list with 2 elements of
        //:     arithmetic or pointer types and verify that it compiles and
        //:     that the resultant list contains the expected values.
        //:
        //: 2 Range constructor (*NOT* the C++11 'std::initializer_list'
        //:   c'tor):
        //:
        //:   1 We will create objects of varying sizes containing default
        //:     values, and insert a range containing distinct values as
        //:     argument.  Perform the above tests:
        //:
        //:     o From the parameterized 'CONTAINER::const_iterator'.
        //:
        //:     o With and without passing in an allocator.
        //:
        //:     o In the presence of exceptions during memory allocations using
        //:       a 'bslma_TestAllocator' and varying its *allocation* *limit*.
        //:
        //:   2 Use basic accessors to verify
        //:
        //:     o size
        //:
        //:     o element value at each index position { 0 .. length - 1 }.
        //
        // Testing:
        //   list(size_type n);
        //   list(size_type n, const T& value = T(), const A& a = A());
        //   template <class iter> list(iter f, iter l, const A& a = A());
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING INITIAL-LENGTH, RANGE CONSTRUCTORS\n"
                            "==========================================\n");

        if (verbose) printf("TESTING INITIAL-LENGTH CONSTRUCTORS\n"
                            "===================================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test12_initialLengthConstructor,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      test12_initialLengthConstructor,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test12_initialLengthConstructor,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        break;
        if (verbose) printf("Testing overloading disambiguation\n");
        {
            // 'n' and 'v' are identical arithmetic types.  Make sure overload
            // resolution doesn't try to call the iterator-range 'insert'.

            {
                size_t n = 2;
                size_t v = 99;

                bsl::list<size_t>  x(n, v);
                bsl::list<size_t>& X = x;

                ASSERT(X.size()  == n);
                ASSERT(X.front() == v);
                ASSERT(X.back()  == v);
            }

            {
                unsigned char n = 2;
                unsigned char v = 99;

                bsl::list<IntWrapper>  x(n, v);
                bsl::list<IntWrapper>& X = x;

                ASSERT(X.size()  == n);
                ASSERT(X.front() == v);
                ASSERT(X.back()  == v);
            }

            {
                size_t n = 2;
                int    v = 99;

                bsl::list<IntWrapper>  x(n, v);
                bsl::list<IntWrapper>& X = x;

                ASSERT(X.size()  == n);
                ASSERT(X.front() == v);
                ASSERT(X.back()  == v);
            }

            {
                TestEnum n = TWO;
                TestEnum v = NINETYNINE;

                bsl::list<IntWrapper>  x(n, v);
                bsl::list<IntWrapper>& X = x;

                ASSERT(X.size()  == (size_t)n);
                ASSERT(X.front() == v);
                ASSERT(X.back()  == v);
            }

            // 'n' is an 'int' and 'v' is a zero 'int' literal (which is also a
            // null pointer literal).  Make sure that it is correctly treated
            // as a pointer.
            {
                int   n = 2;
                char *v = 0;

                bsl::list<char*>   x(n, 0);  // Literal null, acts like an int.
                bsl::list<char*>&  X = x;

                ASSERT(X.size()  == (size_t)n);
                ASSERT(X.front() == v);
                ASSERT(X.back()  == v);
            }
        }

        if (verbose) printf("TESTING RANGE CONSTRUCTORS\n"
                            "==========================\n");

        RUN_EACH_TYPE(TestDriver2,
                      test12_constructorRange,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      test12_constructorRange,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
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
                   "See 'bslstl_list.0.t.cpp' for the test plan.\n",
                   test);
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
