// bsltf_stdallocatoradaptor.h                                        -*-C++-*-
#ifndef INCLUDED_BSLTF_STDALLOCATORADAPTOR
#define INCLUDED_BSLTF_STDALLOCATORADAPTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism to propagate standard allocator state.
//
//@CLASSES:
// bsltf::StdAllocatorAdaptor: adaptor that propagates allocator state
//
//@DESCRIPTION: This component provides an allocator adaptor class template,
// 'bsltf::StdAllocatorAdaptor', that mostly delegates operations to an
// allocator object of a (template parameter) allocator type, except that it
// enables the propagation of the (stateful) allocator object to constructed
// elements, if appropriate.  This class template enables reuse of test cases
// in higher level components (e.g., containers) written first using
// 'bslma::Allocator' and 'bslma::TestAllocator' to also test correct
// allocation using a C++ standard style allocator.
//
// StdAllocatorAdaptor' defines the minimal interface needed in order to comply
// with section 17.6.3.5 ([allocator.requirements]) of the C++11 standard. This
// class is similar to the 'scoped_allocator_adaptor' class template that is
// part of the C++11 standard, except that this adaptor does not support
// multiple levels of allocators (i.e., it is equivalent to the
// 'scoped_allocator_adaptor' with a single allocator).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Allocator Propagation
/// - - - - - - - - - - - - - - - -
// 'bslma::ConstructionUtil' propagates 'bslma::Allocator', wrapped by C++
// standard style allocator, to the constructor, if type, being constructed,
// supports 'UsesBslmaAllocator' trait.  'bsltf::StdAllocatorAdaptor' is used
// in test drivers to get the same behavior for the types that do not support
// that trait.
//
// Suppose, we want to adopt a test for a component that uses 'bslma'-style
// allocation to test that this component correctly works with standard
// allocators.  For simplicity the test below constructs an object of the
// (template parameter) type 'TYPE' by calling allocator's 'construct' method.
// We want to test that allocator is correctly propagated to the object
// constructor.  First, we define the test implementation:
//..
//  template<class TYPE, class ALLOC = bsl::allocator<TYPE> >
//  class TestDriver
//  {
//    public:
//      static void testCase()
//      {
//          bslma::TestAllocator oa("object");
//          ALLOC                xoa(&oa);
//
//          bsls::ObjectBuffer<TYPE> buffer;
//
//          xoa.construct(buffer.address(), 1);
//
//          bslma::DestructorGuard<TYPE> guard(&buffer.object());
//
//          const TYPE& X = buffer.object();
//
//          assert(1   == X.data());
//          assert(&oa == X.allocator());
//      }
//  };
//..
// Now, parametrize 'TestDriver' class with 'StdAllocatorAdaptor' explicitly
// to expand 'testCase' behavior for types, that don't support bslma
// allocators:
//..
//  template<class TYPE>
//  class StdBslmaTestDriver : public TestDriver<TYPE,
//                          bsltf::StdAllocatorAdaptor<bsl::allocator<TYPE> > >
//  {
//  };
//..
// Finally, run the test for types that use 'bslma' and standard allocators:
//..
//  TestDriver<AllocTestType>::testCase();
//  StdBslmaTestDriver<StdAllocTestType<bsl::allocator<int> > >::testCase();
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATORTRAITS
#include <bslma_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLMF_USESALLOCATOR
#include <bslmf_usesallocator.h>
#endif

#ifndef INCLUDED_BSLMF_UTIL
#include <bslmf_util.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_NEW
#include <new>
#define INCLUDED_NEW
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>
#define INCLUDED_STDDEF_H
#endif

namespace BloombergLP {
namespace bsltf {

                        // =========================
                        // class StdAllocatorAdaptor
                        // =========================

template <class ALLOCATOR>
class StdAllocatorAdaptor : public ALLOCATOR {
    // This class template provides the facade of an allocator but mostly
    // delegates operations to the allocator object (of template parameter
    // type) it adapts, except that it enables the propagation of the
    // (stateful) allocator object to constructed elements, if appropriate.

    // PRIVATE TYPES
    typedef bsl::allocator_traits<ALLOCATOR> AllocatorTraits;

    // PRIVATE MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
    template <class ELEMENT_TYPE, class... Args>
    void privateConstruct(bsl::true_type,
                          ELEMENT_TYPE *elemAddr,
                          Args&&...     arguments);
        // Create an object of the (template parameter) 'ELEMENT_TYPE' at the
        // specified 'elemAddr', forwarding the allocator managed by this
        // adaptor and the specified (variable number of) 'arguments' to the
        // corresponding constructor of 'ELEMENT_TYPE'.

    template <class ELEMENT_TYPE, class... Args>
    void privateConstruct(bsl::false_type,
                          ELEMENT_TYPE *elemAddr,
                          Args&&...     arguments);
        // Create an object of the (template parameter) 'ELEMENT_TYPE' at the
        // specified 'elemAddr', forwarding the specified (variable number of)
        // 'arguments' to the corresponding constructor of 'ELEMENT_TYPE'.

#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bsltf_stdallocatoradaptor.h
    template <class ELEMENT_TYPE>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p);

    template <class ELEMENT_TYPE, class Args_01>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12,
                                  class Args_13>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12,
                                  class Args_13,
                                  class Args_14>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14);


    template <class ELEMENT_TYPE>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p);

    template <class ELEMENT_TYPE, class Args_01>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12,
                                  class Args_13>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12,
                                  class Args_13,
                                  class Args_14>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class ELEMENT_TYPE, class... Args>
    void
    privateConstruct(bsl::true_type, ELEMENT_TYPE *p,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);

    template <class ELEMENT_TYPE, class... Args>
    void
    privateConstruct(bsl::false_type, ELEMENT_TYPE *p,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);

// }}} END GENERATED CODE
#endif

  public:
    // PUBLIC TYPES
    typedef typename ALLOCATOR::size_type       size_type;
    typedef typename ALLOCATOR::difference_type difference_type;
    typedef typename ALLOCATOR::pointer         pointer;
    typedef typename ALLOCATOR::const_pointer   const_pointer;
    typedef typename ALLOCATOR::reference       reference;
    typedef typename ALLOCATOR::const_reference const_reference;
    typedef typename ALLOCATOR::value_type      value_type;

    template <class BDE_OTHER_TYPE>
    struct rebind
    {
        // This nested 'struct' template, parameterized by some
        // 'BDE_OTHER_TYPE', provides a namespace for an 'other' type alias,
        // which is an allocator type following the same template as this one
        // but that allocates elements of 'BDE_OTHER_TYPE'.  Note that this
        // allocator type is convertible to and from 'other' for any
        // 'BDE_OTHER_TYPE' including 'void'.

        typedef StdAllocatorAdaptor<
             typename ALLOCATOR::template rebind<BDE_OTHER_TYPE>::other> other;
    };

    // CREATORS
    StdAllocatorAdaptor();
        // Create a standard allocator adaptor object for a default-constructed
        // allocator object of the (template parameter) type 'ALLOCATOR'.

    StdAllocatorAdaptor(const ALLOCATOR& allocator);
        // Create a standard allocator adaptor object for the specified
        // 'allocator' of the (template parameter) type 'ALLOCATOR'.

    template <class ANY_TYPE>
    StdAllocatorAdaptor(const StdAllocatorAdaptor<ANY_TYPE>& other);
        // Create a copy of the specified 'other' allocator adaptor.

    //! StdAllocatorAdaptor(const StdAllocatorAdaptor& original) = default;
        // Create a copy of the specified 'original' allocator adaptor.

    //! ~StdAllocatorAdaptor() = default;
        // Destroy this object.

    // MANIPULATORS
    //! StdAllocatorAdaptor& operator=(const StdAllocatorAdaptor& rhs) =
    //!                                                                default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
    template <class ELEMENT_TYPE, class... Args>
    void construct(ELEMENT_TYPE *address, Args&&... arguments);
        // Construct an object of the (template parameter) 'ELEMENT_TYPE', by
        // forwarding the allocator instance associated with this allocator
        // adaptor, if appropriate, and the specified (variable number of)
        // 'arguments' to the corresponding constructor of 'ELEMENT_TYPE', at
        // the specified uninitialized memory 'address'.  The behavior is
        // undefined unless 'address' is properly aligned for objects of
        // 'ELEMENT_TYPE'.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bsltf_stdallocatoradaptor.h
    template <class ELEMENT_TYPE>
    void construct(ELEMENT_TYPE *address);

    template <class ELEMENT_TYPE, class Args_01>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12,
                                  class Args_13>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12,
                                  class Args_13,
                                  class Args_14>
    void construct(ELEMENT_TYPE *address,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class ELEMENT_TYPE, class... Args>
    void construct(ELEMENT_TYPE *address,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
// }}} END GENERATED CODE
#endif

    // ACCESSORS
    const ALLOCATOR& allocator() const;
        // Return a reference to the non-modifiable allocator instance
        // associated with this standard allocator adaptor.

    StdAllocatorAdaptor<ALLOCATOR>
                                 select_on_container_copy_construction() const;
        // Return an allocator adaptor for the allocator object returned by the
        // 'select_on_container_copy_construction' class method in the
        // 'allocator_traits' class template for the allocator object, of the
        // (template parameter) type 'ALLOCATOR', associated with this adaptor.
        // The 'allocator_traits' class template presumably delegates this call
        // to the allocator object if such an operation is supported by the
        // 'ALLOCATOR' type, or provides a suitable default behavior if such an
        // operation is not supported.
};

// FREE OPERATORS
template <class TYPE1, class TYPE2>
bool operator==(const StdAllocatorAdaptor<TYPE1>& lhs,
                const StdAllocatorAdaptor<TYPE2>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' adaptors are equal and
    // 'false' otherwise.  Two allocator adaptor instances are equal if their
    // associated allocator instances are equal.

template <class TYPE1, class TYPE2>
bool operator!=(const StdAllocatorAdaptor<TYPE1>& lhs,
                const StdAllocatorAdaptor<TYPE2>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' adaptors are not equal
    // and 'false' otherwise.  Two allocator adaptor instances are not equal if
    // their associated allocator instances are not equal.

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class StdAllocatorAdaptor
                        // -------------------------

// PRIVATE MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
template <class ALLOCATOR>
template <class ELEMENT_TYPE, class... Args>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(
                                                     bsl::true_type,
                                                     ELEMENT_TYPE   *address,
                                                     Args&&...       arguments)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...,
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class... Args>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(
                                                    bsl::false_type,
                                                    ELEMENT_TYPE    *address,
                                                    Args&&...        arguments)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
}

#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bsltf_stdallocatoradaptor.h
template <class ALLOCATOR>
template <class ELEMENT_TYPE>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address)
{
    AllocatorTraits::construct(
        *this,
        address,
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
        BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11),
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
        BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11),
        BSLS_COMPILERFEATURES_FORWARD(Args_12, arguments_12),
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12,
                              class Args_13>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
        BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11),
        BSLS_COMPILERFEATURES_FORWARD(Args_12, arguments_12),
        BSLS_COMPILERFEATURES_FORWARD(Args_13, arguments_13),
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12,
                              class Args_13,
                              class Args_14>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
        BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11),
        BSLS_COMPILERFEATURES_FORWARD(Args_12, arguments_12),
        BSLS_COMPILERFEATURES_FORWARD(Args_13, arguments_13),
        BSLS_COMPILERFEATURES_FORWARD(Args_14, arguments_14),
        *this);
}


template <class ALLOCATOR>
template <class ELEMENT_TYPE>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address)
{
    AllocatorTraits::construct(
        *this, address);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
                        BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
                        BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11),
                        BSLS_COMPILERFEATURES_FORWARD(Args_12, arguments_12));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12,
                              class Args_13>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
                        BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11),
                        BSLS_COMPILERFEATURES_FORWARD(Args_12, arguments_12),
                        BSLS_COMPILERFEATURES_FORWARD(Args_13, arguments_13));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12,
                              class Args_13,
                              class Args_14>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
                        BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11),
                        BSLS_COMPILERFEATURES_FORWARD(Args_12, arguments_12),
                        BSLS_COMPILERFEATURES_FORWARD(Args_13, arguments_13),
                        BSLS_COMPILERFEATURES_FORWARD(Args_14, arguments_14));
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class ALLOCATOR>
template <class ELEMENT_TYPE, class... Args>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::true_type,
                                                      ELEMENT_TYPE *address,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    AllocatorTraits::construct(
        *this,
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...,
        *this);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class... Args>
inline
void StdAllocatorAdaptor<ALLOCATOR>::privateConstruct(bsl::false_type,
                                                      ELEMENT_TYPE *address,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    AllocatorTraits::construct(
        *this, address, BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
}
// }}} END GENERATED CODE
#endif

// CREATORS
template <class ALLOCATOR>
inline
StdAllocatorAdaptor<ALLOCATOR>::StdAllocatorAdaptor()
: ALLOCATOR()
{
}

template <class ALLOCATOR>
inline
StdAllocatorAdaptor<ALLOCATOR>::StdAllocatorAdaptor(const ALLOCATOR& allocator)
: ALLOCATOR(allocator)
{
}

template <class ALLOCATOR>
template <class ANY_TYPE>
inline
StdAllocatorAdaptor<ALLOCATOR>::StdAllocatorAdaptor(
                                    const StdAllocatorAdaptor<ANY_TYPE>& other)
: ALLOCATOR(other)
{
}

// MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
template <class ALLOCATOR>
template <class ELEMENT_TYPE, class... Args>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                                          Args&&...     arguments)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bsltf_stdallocatoradaptor.h
template <class ALLOCATOR>
template <class ELEMENT_TYPE>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address);
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
        BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
        BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11),
        BSLS_COMPILERFEATURES_FORWARD(Args_12, arguments_12));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12,
                              class Args_13>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
        BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11),
        BSLS_COMPILERFEATURES_FORWARD(Args_12, arguments_12),
        BSLS_COMPILERFEATURES_FORWARD(Args_13, arguments_13));
}

template <class ALLOCATOR>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12,
                              class Args_13,
                              class Args_14>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
        BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
        BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
        BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
        BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
        BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
        BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
        BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
        BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
        BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
        BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11),
        BSLS_COMPILERFEATURES_FORWARD(Args_12, arguments_12),
        BSLS_COMPILERFEATURES_FORWARD(Args_13, arguments_13),
        BSLS_COMPILERFEATURES_FORWARD(Args_14, arguments_14));
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class ALLOCATOR>
template <class ELEMENT_TYPE, class... Args>
inline void
StdAllocatorAdaptor<ALLOCATOR>::construct(ELEMENT_TYPE *address,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    privateConstruct(
        typename bsl::uses_allocator<
            ELEMENT_TYPE,
            StdAllocatorAdaptor<typename ALLOCATOR::template rebind<
                ELEMENT_TYPE>::other> >::type(),
        address,
        BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
}
// }}} END GENERATED CODE
#endif

// ACCESSORS
template <class ALLOCATOR>
inline
const ALLOCATOR& StdAllocatorAdaptor<ALLOCATOR>::allocator() const
{
    return *this;
}

template <class ALLOCATOR>
inline
StdAllocatorAdaptor<ALLOCATOR>
StdAllocatorAdaptor<ALLOCATOR>::select_on_container_copy_construction() const
{
    return StdAllocatorAdaptor<ALLOCATOR>(
        AllocatorTraits::select_on_container_copy_construction(
            *reinterpret_cast<const ALLOCATOR *>(this)));
}

// FREE OPERATORS
template <class TYPE1, class TYPE2>
inline
bool operator==(const bsltf::StdAllocatorAdaptor<TYPE1>& lhs,
                const bsltf::StdAllocatorAdaptor<TYPE2>& rhs)
{
    return lhs.allocator() == rhs.allocator();
}

template <class TYPE1, class TYPE2>
inline
bool operator!=(const bsltf::StdAllocatorAdaptor<TYPE1>& lhs,
                const bsltf::StdAllocatorAdaptor<TYPE2>& rhs)
{
    return lhs.allocator() != rhs.allocator();
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
