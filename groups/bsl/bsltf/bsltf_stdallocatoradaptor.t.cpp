// bsltf_stdallocatoradaptor.t.cpp                                    -*-C++-*-
#include <bsltf_stdallocatoradaptor.h>
#include <bsltf_simpletesttype.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace BloombergLP::bsltf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a single enumeration supporting
// conversion from integral values (0 to 127) to and from the enumeration.
//
//-----------------------------------------------------------------------------
// TYPES
// [ 1] BREATHING TEST
//-----------------------------------------------------------------------------

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class ALLOC>
class Proctor
{
    typedef bsl::allocator_traits<ALLOC> AllocatorTraits;
    typedef typename bsl::allocator_traits<ALLOC>::pointer pointer;
    ALLOC d_allocator;
    pointer d_data_p;

  public:
    Proctor(const ALLOC& a, pointer p) 
    : d_allocator(a)
    , d_data_p(p)
    {
    }

    ~Proctor() 
    {
        if (d_data_p) {
            AllocatorTraits::deallocate(d_allocator, d_data_p, 1);
        }
    }

    void release()
    {
        d_data_p = pointer();
    }
};

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
class Container
{
    typedef bsl::allocator_traits<ALLOC> AllocatorTraits;

    TYPE *d_value_p;
    ALLOC d_allocator;

  public:
    // TRAITS

    BSLMF_NESTED_TRAIT_DECLARATION_IF(Container, bslma::UsesBslmaAllocator,
                                      (bsl::is_convertible<bslma::Allocator *,
                                                           ALLOC>::value));
    typedef TYPE value_type;
    typedef ALLOC allocator_type;

    // CREATORS

    explicit Container(const ALLOC& allocator = ALLOC())
    : d_allocator(allocator)
    {
        d_value_p = AllocatorTraits::allocate(d_allocator, 1);
        Proctor<ALLOC> p(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator, d_value_p);
        p.release();
    }

    Container(const TYPE& value, const ALLOC& allocator)
    : d_allocator(allocator)
    {
        d_value_p = AllocatorTraits::allocate(d_allocator, 1);
        Proctor<ALLOC> p(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator, d_value_p, value);
        p.release();
    }

    Container(const Container& original)
    : d_allocator(AllocatorTraits::select_on_container_copy_construction(
                                          original.d_allocator))
    {
        d_value_p = AllocatorTraits::allocate(d_allocator, 1);
        Proctor<ALLOC> p(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator,
                                   d_value_p,
                                   *original.d_value_p);
        p.release();
    }

    Container(bslmf::MovableRef<Container> original)
    : d_allocator(bslmf::MovableRefUtil::access(original).get_allocator())
    {
        Container& lvalue = original;
        d_value_p = lvalue.d_value_p;
        lvalue.d_value_p = 0;
    }

    Container(const Container& original, const ALLOC& allocator)
    : d_allocator(allocator)
    {
        d_value_p = AllocatorTraits::allocate(d_allocator, 1);
        Proctor<ALLOC> p(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator,
                                   d_value_p,
                                   *original.d_value_p);
        p.release();
    }

    Container(bslmf::MovableRef<Container> original, const ALLOC& allocator)
    : d_allocator(allocator)
    {
        Container& lvalue = original;
        if (d_allocator == lvalue.d_allocator) {
            d_value_p = lvalue.d_value_p;
            lvalue.d_value_p = 0;
        }
        else {
            d_value_p = AllocatorTraits::allocate(d_allocator, 1);
            Proctor<ALLOC> p(d_allocator, d_value_p);
            AllocatorTraits::construct(d_allocator,
                                       d_value_p, 
                                       *original.d_value_p);
            p.release();
        }
    }

    ~Container()
    {
        if (d_value_p) {
            AllocatorTraits::destroy(d_allocator, d_value_p);
            AllocatorTraits::deallocate(d_allocator, d_value_p, 1);
        }
    }

    // MANIPULATORS
    void insert(const TYPE& value)
    {
        AllocatorTraits::destroy(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator, d_value_p,
                                    value);
    }

    template <class P>
    void insert(BSLS_COMPILERFEATURES_FORWARD_REF(P) value)
    {
        AllocatorTraits::destroy(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator, d_value_p,
                                   BSLS_COMPILERFEATURES_FORWARD(P,value));
    }

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template <class... Args>
    TYPE& emplace(Args&&... arguments)
    {
        AllocatorTraits::destroy(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator, d_value_p,
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
        return *d_value_p;
    }
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl main40.cpp
    TYPE& emplace()
    {
        AllocatorTraits::destroy(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator, d_value_p);
        return *d_value_p;
    }

    template <class Args_1>
    TYPE& emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_1) arguments_1)
    {
        AllocatorTraits::destroy(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator, d_value_p,
                            BSLS_COMPILERFEATURES_FORWARD(Args_1,arguments_1));
        return *d_value_p;
    }

    template <class Args_1,
              class Args_2>
    TYPE& emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_1) arguments_1,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_2) arguments_2)
    {
        AllocatorTraits::destroy(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator, d_value_p,
                            BSLS_COMPILERFEATURES_FORWARD(Args_1,arguments_1),
                            BSLS_COMPILERFEATURES_FORWARD(Args_2,arguments_2));
        return *d_value_p;
    }

    template <class Args_1,
              class Args_2,
              class Args_3>
    TYPE& emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_1) arguments_1,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_2) arguments_2,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_3) arguments_3)
    {
        AllocatorTraits::destroy(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator, d_value_p,
                            BSLS_COMPILERFEATURES_FORWARD(Args_1,arguments_1),
                            BSLS_COMPILERFEATURES_FORWARD(Args_2,arguments_2),
                            BSLS_COMPILERFEATURES_FORWARD(Args_3,arguments_3));
        return *d_value_p;
    }

    template <class Args_1,
              class Args_2,
              class Args_3,
              class Args_4>
    TYPE& emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_1) arguments_1,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_2) arguments_2,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_3) arguments_3,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_4) arguments_4)
    {
        AllocatorTraits::destroy(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator, d_value_p,
                            BSLS_COMPILERFEATURES_FORWARD(Args_1,arguments_1),
                            BSLS_COMPILERFEATURES_FORWARD(Args_2,arguments_2),
                            BSLS_COMPILERFEATURES_FORWARD(Args_3,arguments_3),
                            BSLS_COMPILERFEATURES_FORWARD(Args_4,arguments_4));
        return *d_value_p;
    }

    template <class Args_1,
              class Args_2,
              class Args_3,
              class Args_4,
              class Args_5>
    TYPE& emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_1) arguments_1,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_2) arguments_2,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_3) arguments_3,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_4) arguments_4,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_5) arguments_5)
    {
        AllocatorTraits::destroy(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator, d_value_p,
                            BSLS_COMPILERFEATURES_FORWARD(Args_1,arguments_1),
                            BSLS_COMPILERFEATURES_FORWARD(Args_2,arguments_2),
                            BSLS_COMPILERFEATURES_FORWARD(Args_3,arguments_3),
                            BSLS_COMPILERFEATURES_FORWARD(Args_4,arguments_4),
                            BSLS_COMPILERFEATURES_FORWARD(Args_5,arguments_5));
        return *d_value_p;
    }

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... Args>
    TYPE& emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
    {
        AllocatorTraits::destroy(d_allocator, d_value_p);
        AllocatorTraits::construct(d_allocator, d_value_p,
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
        return *d_value_p;
    }
// }}} END GENERATED CODE
#endif

    allocator_type get_allocator() const { return d_allocator; }

    TYPE *front() { return d_value_p; }

    // ACCESSORS

    const TYPE *front() const { return d_value_p; }
};

template <class TYPE, class ALLOC>
class AllocTestType
{
    ALLOC  d_allocator;
    TYPE  *d_value_p;
  public:
    typedef ALLOC allocator_type;

    // CREATORS
    AllocTestType(const ALLOC& a = ALLOC())
    : d_allocator(a)
    {
        d_value_p = bsl::allocator_traits<ALLOC>::allocate(d_allocator, 1);
        bsl::allocator_traits<ALLOC>::construct(d_allocator, d_value_p);
    }

    AllocTestType(const TYPE& value, const ALLOC& a = ALLOC())
    : d_allocator(a)
    {
        d_value_p = bsl::allocator_traits<ALLOC>::allocate(d_allocator, 1);
        bsl::allocator_traits<ALLOC>::construct(d_allocator, d_value_p, value);
    }

    AllocTestType(bslmf::MovableRef<AllocTestType> original)
    : d_allocator(bslmf::MovableRefUtil::access(original).d_allocator)
    {
        AllocTestType& lvalue = original;
        d_value_p = lvalue.d_value_p;
        lvalue.d_value_p = 0;
    }

    AllocTestType(bslmf::MovableRef<AllocTestType> original, const ALLOC& a)
    : d_allocator(a)
    {
        AllocTestType& lvalue = original;
        if (d_allocator == lvalue.d_allocator) {
            d_value_p = lvalue.d_value_p;
            lvalue.d_value_p = 0;
        }
        else {
            d_value_p = bsl::allocator_traits<ALLOC>::allocate(d_allocator, 1);
            bsl::allocator_traits<ALLOC>::construct(
                d_allocator,
                d_value_p,
                bslmf::MovableRefUtil::move(*lvalue.d_value_p));
            // TBD: need moved-from flag
        }
    }

    AllocTestType(const AllocTestType& original)
    : d_allocator(bsl::allocator_traits<ALLOC>::
                   select_on_container_copy_construction(original.d_allocator))
    {
        d_value_p = bsl::allocator_traits<ALLOC>::allocate(d_allocator, 1);
        bsl::allocator_traits<ALLOC>::construct(
            d_allocator, d_value_p, *original.d_value_p);
    }

    AllocTestType(const AllocTestType& original, const ALLOC& a)
    : d_allocator(a)
    {
        d_value_p = bsl::allocator_traits<ALLOC>::allocate(d_allocator, 1);
        bsl::allocator_traits<ALLOC>::construct(
            d_allocator, d_value_p, *original.d_value_p);
    }

    ~AllocTestType() 
    {
        if (d_value_p) {
            bsl::allocator_traits<ALLOC>::destroy(d_allocator, d_value_p);
            bsl::allocator_traits<ALLOC>::deallocate(d_allocator, d_value_p, 1);
        }
    }

    // MANIPULATORS
    AllocTestType& operator=(const AllocTestType& rhs)
    {
        if (bsl::allocator_traits<ALLOC>::
                                 propagate_on_container_copy_assignment::value)
        {
            AllocTestType other(rhs, rhs.allocator());
            ALLOC tmp = d_allocator;
            d_allocator = other.d_allocator;
            other.d_allocator = tmp;
            TYPE *t = d_value_p;
            d_value_p = other.d_value_p;
            other.d_value_p = t;
        }
        else {
            AllocTestType other(rhs, d_allocator);
            TYPE *t = d_value_p;
            d_value_p = other.d_value_p;
            other.d_value_p = t;
        }
        return *this;
    }

    AllocTestType& operator=(bslmf::MovableRef<AllocTestType> rhs)
    {
        AllocTestType& lvalue = rhs;
        if (this != &lvalue) {
            if (d_allocator == lvalue.d_allocator) {
                d_value_p = lvalue.d_value_p;
                lvalue.d_value_p = 0;
            }
            else if (bsl::allocator_traits<ALLOC>::
                                 propagate_on_container_move_assignment::value)
            {
                AllocTestType other(bslmf::MovableRefUtil::move(lvalue));
                ALLOC tmp = d_allocator;
                d_allocator = other.d_allocator;
                other.d_allocator = tmp;
                TYPE *t = d_value_p;
                d_value_p = other.d_value_p;
                other.d_value_p = t;
            }
            else {
                AllocTestType other(bslmf::MovableRefUtil::move(lvalue),
                                    d_allocator);
                TYPE *t = d_value_p;
                d_value_p = other.d_value_p;
                other.d_value_p = t;
            }
        }
        return *this;
    }

    // ACCESSORS

    const ALLOC& allocator() const 
    {
        return d_allocator;
    }

    const TYPE *value() const
    {
        return d_value_p;
    }
};

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef AllocTestType<int,
                              StdAllocatorAdaptor<bsl::allocator<int> > > U;
        typedef StdAllocatorAdaptor<bsl::allocator<U> > A;
        typedef Container<U, A> C;

        bslma::TestAllocator testAlloc;
        A stdTestAlloc(&testAlloc);

        C c1(stdTestAlloc);
        ASSERTV((bsl::is_same<C::allocator_type, A >::value));
        ASSERTV(c1.get_allocator() == stdTestAlloc);
        ASSERTV(c1.front());
        ASSERTV(c1.front()->allocator() == stdTestAlloc);

        C c2(c1);
        ASSERTV(c2.get_allocator() != c1.get_allocator());
        ASSERTV(c2.get_allocator() == A());
        ASSERTV(c2.front());
        ASSERTV(c2.front()->allocator() == A());

        bslma::TestAllocator testAlloc2;
        A stdTestAlloc2(&testAlloc2);

        C c3(c1, stdTestAlloc2);
        ASSERTV(c3.get_allocator() != c1.get_allocator());
        ASSERTV(c3.get_allocator() == stdTestAlloc2);
        ASSERTV(c3.front());
        ASSERTV(c3.front()->allocator() == stdTestAlloc2);

        c3.emplace(2);
        ASSERTV(c3.front()->value());
        ASSERTV(2 == *c3.front()->value());
        ASSERTV(c3.front()->allocator() == stdTestAlloc2);

        U att(4, stdTestAlloc2);
        ASSERTV(att.value(), att.value());
        ASSERTV(4 == *att.value());
        c3.emplace(bslmf::MovableRefUtil::move(att));
        ASSERTV(c3.front()->value());
        ASSERTV(4 == *c3.front()->value());
        ASSERTV(att.value(), 0 == att.value());
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
// Copyright 2016 Bloomberg Finance L.P.
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
