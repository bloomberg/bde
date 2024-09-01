// bslma_constructionutil.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMA_CONSTRUCTIONUTIL
#define INCLUDED_BSLMA_CONSTRUCTIONUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide methods to construct arbitrarily-typed objects uniformly.
//
//@CLASSES:
//  bslma::ConstructionUtil: namespace for methods to construct objects
//
//@SEE_ALSO: bslma_allocator, bslma_bslallocator, bslma_destructionutil
//
//@DESCRIPTION: This component provides a `struct`, `bslma::ConstructionUtil`,
// that serves as a namespace for utility functions to construct objects of an
// arbitrary (template parameter) type, given an allocator and an arbitrary
// number of arguments.  These functions are useful for uniformly constructing
// an object without concern for whether the object's type is allocator-aware
// (AA) and, if so, whether the allocator is passed at the end of the argument
// list (trailing allocator-argument convention) or at the start of the
// argument list, preceeded by `bsl::allocator_arg` (leading allocator-argument
// convention).  If the type being constructed is not AA or if a non-allocator
// scalar such as an `int` or `void *` is passed to these methods, the
// allocator argument is discarded and a non-allocator constructor is invoked.
//
// An additional `destructiveMove` method moves an object to a new address and
// destroys the object at the original address, often doing so as an efficient
// `memcpy` rather than separate move-destroy operations.
//
// *Legacy-AA* (types with constructors having a parameter of type
// `bslma::Allocator *`) and *bsl-AA* types (types with constructors having a
// parameter of type `bsl::allocator<>`) are handled interchangeably by the
// functions in this component, accepting either a `bslma::Allocator*` or a
// `bsl::allocator` argument and then passing the appropriate type to the
// constructed type.  Thus *bsl-AA* code can work seamlessly with *legacy-AA*
// code, allowing for a smooth transition from the old model to the new one.
// Note that this component does not directly use `bsl::allocator` or
// `bsl::polymorphic_allocator` because such use would cause a circular
// dependancy; an allocator is considered bsl-like if it is implicitly
// convertible from `bslma::Allocator *` and has a `mechanism` method that
// returns a `bslma::Allocator *` and an allocator is considered pmr-like if it
// is convertible from `bsl::memory_resource *` (and therefore also convertible
// from `bslma::Allocator *`.
//
// The `construct` method provided here has roughly the same functionality as
// the C++20 library function `std::uninitialized_construct_using_allocator`
// and the `make` method has roughly the same functionality as
// `std::make_obj_using_allocator`, with the following differences:
//
// * As described above, *Legacy-AA* types are treated as interchangle with
//   *bsl-AA* types and `bslma::Allocator *` is treated as interchangeable
//   with `bsl::allocator`.
// * The methods in this component accept a non-allocator scalar (e.g., `int`
//   or `void *`) instead of an allocator to indicate that no allocator should
//   be supplied to the object constructor, even if it is AA.
// * The `std` functions will ignore the allocator if it is incompatible with
//   the object type being constructed.  The methods in this component,
//   conversely, will ignore the allocator only for *non-AA* and *stl-AA*
//   types.  For *legacy-AA*, *bsl-AA*, and *pmr-AA* types, an incompatible
//   allocator will result in a compilation failure.  This special case exists
//   to avoid subtle errors in the Bloomberg code base where a bad allocator
//   argument is used in a situation where allocator propagation is expected.
// * This component does not provide special handling for `std::pair`.
//
///Type Traits
///-----------
// The facilities in this component query several type traits of the `TYPE`
// being constructed, using that information as follows:
// ```
// Trait                       How Used
// --------------------------- -----------------------------------------------
// bslma::UsesBslmaAllocator   If true, the allocator being passed in must be
//                             either convertible to 'bslma::Allocator *' or
//                             else have a 'mechanism' accessor.  If false,
//                             the allocator argument must be convertible to
//                             the 'TYPE::allocator_type'.
//
// bslma::HasAllocatorType     If true, 'TYPE::allocator_type' is assumed to
//                             the type of allocator accepted by 'TYPE''s
//                             constructors.
//
// bsl::uses_allocator<TYPE,A> If true, then 'A' can be used as an allocator
//                             to construct 'TYPE'.
//
// bslmf::UsesAllocatorArgT    If true, an allocator argument is passed as the
//                             second argument to the constructor, preceeded
//                             by a 'bsl::allocator_arg' tag; otherwise the
//                             allocator is passed as the last argument to the
//                             constructor.
//
// bslmf::IsBitwiseMoveable    If true, 'destructiveMove' is implemented as a
//                             simple 'memcpy', rather than as a move-destroy
//                             sequence.
// ```
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using `bslma::ConstructionUtil` to Implement a Container
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the intended use of `bslma::ConstructionUtil` to
// implement a simple container class that uses an instance of `bsl::allocator`
// for memory management.
//
// First, because allocation and construction are done in two separate steps,
// we need to define a proctor type that will deallocate the allocated memory
// in case the constructor throws an exception:
// ```
// #include <bslma_bslallocator.h>
//
// template <class TYPE>
// class MyContainerProctor {
//     // This class implements a proctor to release memory allocated during
//     // the construction of a 'MyContainer' object if the constructor for
//     // the container's data element throws an exception.  Such a proctor
//     // should be 'release'd once the element is safely constructed.
//
//     // DATA
//     bsl::allocator<TYPE>  d_allocator;
//     TYPE                 *d_address_p;    // proctored memory
//
//   private:
//     // NOT IMPLEMENTED
//     MyContainerProctor(const MyContainerProctor&);             // = delete
//     MyContainerProctor& operator=(const MyContainerProctor&);  // = delete
//
//   public:
//     // CREATORS
//     MyContainerProctor(const bsl::allocator<TYPE> allocator, TYPE *address)
//         // Create a proctor that conditionally manages the memory at the
//         // specified 'address', and that uses the specified 'allocator' to
//         // deallocate the block of memory (if not released -- see
//         // 'release') upon destruction.  The behavior is undefined unless
//         // 'allocator' is non-zero and supplied the memory at 'address'.
//     : d_allocator(allocator)
//     , d_address_p(address)
//     {
//     }
//
//     ~MyContainerProctor()
//         // Destroy this proctor, and deallocate the block of memory it
//         // manages (if any) by invoking the 'deallocate' method of the
//         // allocator that was supplied at construction of this proctor.  If
//         // no memory is currently being managed, this method has no effect.
//     {
//         if (d_address_p) {
//             d_allocator.deallocate(d_address_p, 1);
//         }
//     }
//
//     // MANIPULATORS
//     void release()
//         // Release from management the block of memory currently managed by
//         // this proctor.  If no memory is currently being managed, this
//         // method has no effect.
//     {
//         d_address_p = 0;
//     }
// };
// ```
// Then, we create a container class that holds a single element and uses
// `bsl::allocator` to supply memory:
// ```
// #include <bslma_constructionutil.h>
//
// template <class TYPE>
// class MyContainer {
//     // This class provides a container that always holds exactly one
//     // element, dynamically allocated using the specified 'bslma'
//     // allocator.
//
//     // DATA
//     bsl::allocator<TYPE>  d_allocator;
//     TYPE                 *d_value_p;
//
//     TYPE *createElement();
//     TYPE *createElement(const TYPE& value);
//         // Return the address of a new element that was allocated from this
//         // container's allocator and initialized with the optionally
//         // specified 'value', or default-initialized if 'value' is not
//         // specified.  If 'TYPE' is AA, this container's allocator is used
//         // to construct the new element.
//
//   public:
//     typedef bsl::allocator<TYPE>  allocator_type;
//
//     // CREATORS
//     explicit
//     MyContainer(const allocator_type& allocator = allocator_type())
//         // Create a container with a default-constructed element.
//         // Optionally specify a 'allocator' used to supply memory.
//         : d_allocator(allocator), d_value_p(createElement()) { }
//
//     explicit
//     MyContainer(const TYPE&           value,
//                 const allocator_type& allocator = allocator_type())
//         // Create a container having an element constructed from the
//         // specified 'value'.  Optionally specify an 'allocator' to supply
//         // memory both for the container and for the contained element.
//         : d_allocator(allocator), d_value_p(createElement(value)) { }
//
//     MyContainer(const MyContainer&    original,
//                 const allocator_type& allocator = allocator_type())
//         // Create a container having the same value as the specified
//         // 'original' object.  Optionally specify a 'allocator' used
//         // to supply memory.  If 'allocator' is 0, the currently
//         // installed default allocator is used.
//         : d_allocator(allocator)
//         , d_value_p(createElement(*original.d_value_p)) { }
//
//     ~MyContainer();
//         // Destroy this object.
//
//     // MANIPULATORS
//     MyContainer& operator=(const TYPE& rhs);
//     MyContainer& operator=(const MyContainer& rhs);
//         // Assign to this object the value of the specified 'rhs' object,
//         // and return a reference providing modifiable access to this
//         // object.
//
//     TYPE& front()
//         // Return a non-'const' reference to the element contained in this
//         // object.
//     {
//         return *d_value_p;
//     }
//
//     // ACCESSORS
//     const TYPE& front() const
//         // Return a 'const' reference to the element contained in this
//         // object.
//     {
//         return *d_value_p;
//     }
//
//     allocator_type get_allocator() const
//         // Return the allocator used by this object to supply memory.
//     {
//         return d_allocator;
//     }
//
//     // etc.
// };
// ```
// Next, we implement the private `createElement` members that allocate memory
// and construct a `TYPE` object in the allocated memory.  We perform the
// allocation using the `allocate` method of `bsl::allocator` and the
// construction using the `construct` method of `ConstructionUtil` that
// provides the correct semantics for passing the allocator to the constructed
// object when appropriate:
// ```
// template <class TYPE>
// TYPE *MyContainer<TYPE>::createElement()
// {
//     TYPE *value_p = d_allocator.allocate(1);
//     MyContainerProctor<TYPE> proctor(d_allocator, value_p);
//
//     // Call 'construct' passing the allocator but no constructor
//     // arguments.
//
//     bslma::ConstructionUtil::construct(value_p, d_allocator);
//     proctor.release();
//
//     return value_p;
// }
//
// template <class TYPE>
// TYPE *MyContainer<TYPE>::createElement(const TYPE& value)
// {
//     TYPE *value_p = d_allocator.allocate(1);
//     MyContainerProctor<TYPE> proctor(d_allocator, value_p);
//
//     // Call 'construct' passing the allocator and 'value' arguments.
//
//     bslma::ConstructionUtil::construct(value_p, d_allocator, value);
//     proctor.release();
//
//     return value_p;
// }
// ```
// Now, the destructor destroys the object and deallocates the memory used to
// hold the element using the allocator:
// ```
// template <class TYPE>
// MyContainer<TYPE>::~MyContainer()
// {
//     d_value_p->~TYPE();
//     d_allocator.deallocate(d_value_p, 1);
// }
// ```
// Next, the assignment operator needs to assign the value without modifying
// the allocator.
// ```
// template <class TYPE>
// MyContainer<TYPE>& MyContainer<TYPE>::operator=(const TYPE& rhs)
// {
//     if (&rhs != d_value_p) {
//         *d_value_p = rhs;
//     }
//     return *this;
// }
//
// template <class TYPE>
// MyContainer<TYPE>& MyContainer<TYPE>::operator=(const MyContainer& rhs)
// {
//     return operator=(*rhs.d_value_p);
// }
// ```
// Finally, we perform a simple test of `MyContainer`, instantiating it with
// element type `int`:
// ```
// int main()
// {
//     bslma::TestAllocator testAlloc;
//     MyContainer<int>     C1(123, &testAlloc);
//     assert(C1.get_allocator() == &testAlloc);
//     assert(C1.front()         == 123);
//
//     MyContainer<int> C2(C1);
//     assert(C2.get_allocator() == bslma::Default::defaultAllocator());
//     assert(C2.front()         == 123);
//
//     return 0;
// }
// ```
//
///Example 2: `bslma` Allocator Propagation
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates that `MyContainer` does indeed propagate the
// allocator to its contained element.
//
// First, we create a representative element class, `MyType`.  Unlike the
// `MyContainer` template, `MyType` allocates memory using the
// `bslma::Allocator *` (legacy) allocator model instead of the
// `bsl::allocator` (bsl) allocator model:
// ```
// #include <bslma_default.h>
//
// class MyType {
//
//     // DATA
//     bslma::Allocator *d_allocator_p;
//     int               d_value;
//     // ...
//
//   public:
//     // TRAITS
//     BSLMF_NESTED_TRAIT_DECLARATION(MyType, bslma::UsesBslmaAllocator);
//
//     // CREATORS
//     explicit MyType(bslma::Allocator *basicAllocator = 0)
//         // Create a 'MyType' object having the default value.  Optionally
//         // specify a 'basicAllocator' used to supply memory.  If
//         // 'basicAllocator' is 0, the currently installed default allocator
//         // is used.
//         : d_allocator_p(bslma::Default::allocator(basicAllocator))
//         , d_value()
//     {
//         // ...
//     }
//
//     explicit MyType(int               value,
//                     bslma::Allocator *basicAllocator = 0)
//         // Create a 'MyType' object having the specified 'value'.
//         // Optionally specify a 'basicAllocator' used to supply memory.  If
//         // 'basicAllocator' is 0, the currently installed default allocator
//         // is used.
//         : d_allocator_p(bslma::Default::allocator(basicAllocator))
//         , d_value(value)
//     {
//         // ...
//     }
//
//     MyType(const MyType& original, bslma::Allocator *basicAllocator = 0)
//         // Create a 'MyType' object having the same value as the specified
//         // 'original' object.  Optionally specify a 'basicAllocator' used
//         // to supply memory.  If 'basicAllocator' is 0, the currently
//         // installed default allocator is used.
//     : d_allocator_p(bslma::Default::allocator(basicAllocator))
//     , d_value(original.value())
//     {
//         // ...
//     }
//
//     // ...
//
//     // ACCESSORS
//     bslma::Allocator *allocator() const
//         // Return the allocator used by this object to supply memory.
//     {
//         return d_allocator_p;
//     }
//
//     int value() const
//         // Return the value of this object.
//     {
//         return d_value;
//     }
//
//     // ...
// };
// ```
// Finally, we instantiate `MyContainer` using `MyType` and verify that, when
// we provide an allocator to the constructor of the container, the same
// allocator is passed to the constructor of the contained element.  Because
// the container and the element implement different allocator models, the
// invocation of `bslma::ConstructionUtil::construct` automatically adapts the
// `bsl::allocator` held by the container to a `bslma::Allocator` pointer
// expected by the element.  We also verify that, when the container is
// copy-constructed without supplying an allocator, the copy uses the default
// allocator, not the allocator from the original object.  Moreover, we verify
// that the element stored in the copy also uses the default allocator:
// ```
// int main()
// {
//     bslma::TestAllocator testAlloc;
//     bslma::TestAllocator testAlloc2;
//
//     MyContainer<MyType>  C1(&testAlloc);  // extended default constructor
//     assert(C1.get_allocator()     == &testAlloc);
//     assert(C1.front().allocator() == &testAlloc);
//     assert(C1.front().value()     == 0);
//
//     MyContainer<MyType>  C2(MyType(22), &testAlloc);  // value constructor
//     assert(C2.get_allocator()     == &testAlloc);
//     assert(C2.front().allocator() == &testAlloc);
//     assert(C2.front().value()     == 22);
//
//     MyContainer<MyType> C3(C2);
//     assert(C3.get_allocator()     != C2.get_allocator());
//     assert(C3.get_allocator()     == bslma::Default::defaultAllocator());
//     assert(C3.front().allocator() != C1.front().allocator());
//     assert(C3.front().allocator() == bslma::Default::defaultAllocator());
//     assert(C3.front().value()     == 22);
//
//     MyContainer<MyType> C4(C2, &testAlloc2);
//     assert(C4.get_allocator()     == &testAlloc2);
//     assert(C4.front().allocator() == &testAlloc2);
//     assert(C4.front().value()     == 22);
// }
// ```
//
///Example 3: Constructing into Non-heap Memory
///- - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using `bslma::ConstructionUtil::make` to
// implement a simple wrapper class that contains a single item that might or
// might not use the `bslma` allocator protocol.
//
// First, we define a wrapper class that holds an object and a functor.  The
// functor (known as the *listener*) is called each time the wrapped object is
// changes.  We store the object directly as a member variable, instead of
// using an uninitialized buffer, to avoid a separate construction step:
// ```
// template <class TYPE, class FUNC>
// class MyTriggeredWrapper {
//     // This class is a wrapper around an object of the specified 'TYPE'
//     // that triggers a call to an object, called the "listener", of the
//     // specified 'FUNC' invocable type whenever the wrapped object is
//     // changed.
//
//     // DATA
//     TYPE d_value;
//     FUNC d_listener;
//
//   public:
//     typedef bsl::allocator<> allocator_type;
//
//     // CREATORS
//     explicit
//     MyTriggeredWrapper(const FUNC&           f,
//                        const allocator_type& allocator = allocator_type());
//     MyTriggeredWrapper(const TYPE&           v,
//                        const FUNC&           f,
//                        const allocator_type& allocator = allocator_type());
//         // Create an object with the specified 'f' as the listener to be
//         // called when a change is triggered.  Optionally specify 'v' as
//         // the wrapped value; otherwise the wrapped value is default
//         // constructed.  Optionally specify 'allocator' to supply
//         // memory; otherwise the current default allocator is used.  If
//         // 'TYPE' is not allocator aware, 'allocator' is ignored.
//
//     MyTriggeredWrapper(const MyTriggeredWrapper&  original,
//                        const allocator_type& allocator = allocator_type());
//         // Create a copy of the specified 'original'.  Optionally specify
//         // 'allocator' to supply memory; otherwise the current
//         // default allocator is used.
//
//     ~MyTriggeredWrapper()
//         // Destroy the wrapped object and listener.
//     {
//     }
//
//     // MANIPULATORS
//     MyTriggeredWrapper& operator=(const TYPE& rhs);
//     MyTriggeredWrapper& operator=(const MyTriggeredWrapper& rhs);
//         // Assign to the wrapped value the value of the specified 'rhs',
//         // invoke the listener with the new value, and return a reference
//         // providing modifiable access to this object.  Note that the
//         // listener itself is not assigned.
//
//     void setValue(const TYPE& value);
//         // Set the wrapped value to the specified 'value' and invoke the
//         // listener with the new value.
//
//     // ACCESSORS
//     const TYPE& value() const
//         // Return a reference providing read-only access to the wrapped
//         // value.
//     {
//         return d_value;
//     }
//
//     const FUNC& listener() const
//         // Return a reference providing read-only access to the listener.
//     {
//         return d_listener;
//     }
// };
// ```
// Next, we define the constructors such that they initialize `d_value` using
// the specified allocator if and only if `TYPE` accepts an allocator.  The
// `bslma::ConstructionUtil::make` family of functions encapsulate all of the
// metaprogramming that detects whether `TYPE` uses an allocator and, if so,
// which construction protocol it uses (allocator at the front or at the back
// of the argument list), making all three constructors straightforward:
// ```
// template <class TYPE, class FUNC>
// MyTriggeredWrapper<TYPE, FUNC>::MyTriggeredWrapper(
//                                           const FUNC&           f,
//                                           const allocator_type& allocator)
// : d_value(bslma::ConstructionUtil::make<TYPE>(allocator))
// , d_listener(f)
// {
// }
//
// template <class TYPE, class FUNC>
// MyTriggeredWrapper<TYPE, FUNC>::MyTriggeredWrapper(
//                                           const TYPE&           v,
//                                           const FUNC&           f,
//                                           const allocator_type& allocator)
// : d_value(bslma::ConstructionUtil::make<TYPE>(allocator, v))
// , d_listener(f)
// {
// }
//
// template <class TYPE, class FUNC>
// MyTriggeredWrapper<TYPE, FUNC>::MyTriggeredWrapper(
//                                        const MyTriggeredWrapper& other,
//                                        const allocator_type&     allocator)
// : d_value(bslma::ConstructionUtil::make<TYPE>(allocator, other.value()))
// , d_listener(other.d_listener)
// {
// }
// ```
// Note that, for `d_value` to be constructed with the correct allocator, the
// compiler must construct the result returned from `make` directly into the
// `d_value` variable, an optimization known prior to C++17 as "copy elision".
// This optimization is required by the C++17 standard and is optional in
// pre-2017 standards, but is implemented in all of the C++11 compilers for
// which this component is expected to be used at Bloomberg.
//
// Next, we implement the assignment operators, which simply call `setValue`:
// ```
// template <class TYPE, class FUNC>
// MyTriggeredWrapper<TYPE, FUNC>&
// MyTriggeredWrapper<TYPE, FUNC>::operator=(const TYPE& rhs)
// {
//     setValue(rhs);
//     return *this;
// }
//
// template <class TYPE, class FUNC>
// MyTriggeredWrapper<TYPE, FUNC>&
// MyTriggeredWrapper<TYPE, FUNC>::operator=(const MyTriggeredWrapper& rhs)
// {
//     setValue(rhs.value());
//     return *this;
// }
// ```
// Then, we implement `setValue`, which calls the listener after modifying the
// value:
// ```
// template <class TYPE, class FUNC>
// void MyTriggeredWrapper<TYPE, FUNC>::setValue(const TYPE& value)
// {
//     d_value = value;
//     d_listener(d_value);
// }
// ```
// Finally, we check our work by creating a listener for `MyContainer<int>`
// that stores its last-seen value in a known location and a wrapper around
// `MyContainer<int>` to test it:
// ```
// int lastSeen = 0;
// void myListener(const MyContainer<int>& c)
// {
//     lastSeen = c.front();
// }
//
// int main()
// {
//     bslma::TestAllocator testAlloc;
//     MyTriggeredWrapper<MyContainer<int>,
//                        void (*)(const MyContainer<int>&)>
//                          wrappedContainer(myListener, &testAlloc);
//     assert(&testAlloc == wrappedContainer.value().get_allocator());
//
//     wrappedContainer = MyContainer<int>(99);
//
//     assert(99 == lastSeen);
// }
// ```

#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_destructionutil.h>
#include <bslma_hasallocatortype.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isclass.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isfundamental.h>
#include <bslmf_ismemberpointer.h>
#include <bslmf_ispointer.h>
#include <bslmf_movableref.h>
#include <bslmf_removecv.h>
#include <bslmf_usesallocatorargt.h>
#include <bslmf_util.h>    // 'forward(V)'
#include <bslmf_voidtype.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <stddef.h>
#include <string.h>

#include <new>          // placement 'new'

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Tue Oct  4 14:49:46 2022
// Command line: sim_cpp11_features.pl bslma_constructionutil.h
# define COMPILING_BSLMA_CONSTRUCTIONUTIL_H
# include <bslma_constructionutil_cpp03.h>
# undef COMPILING_BSLMA_CONSTRUCTIONUTIL_H
#else

namespace BloombergLP {
namespace bslma {

// Workaround for optimization issue in xlC that mishandles pointer aliasing.
//   IV56864: ALIASING BEHAVIOUR FOR PLACEMENT NEW
//   http://www-01.ibm.com/support/docview.wss?uid=swg1IV56864
// Place this macro following each use of placement new.  Alternatively,
// compile with xlC_r -qalias=noansi, which reduces optimization opportunities
// across entire translation unit instead of simply across optimization fence.
// Update: issue is fixed in xlC 13.1 (__xlC__ >= 0x0d01).

#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VERSION < 0x0d01
    #define BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX                     \
                             BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE
#else
    #define BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX
#endif

struct ConstructionUtil_Imp;

                          // =======================
                          // struct ConstructionUtil
                          // =======================

/// This `struct` provides a namespace for utility functions that construct
/// elements of (a template parameter) `TARGET_TYPE`.
struct ConstructionUtil {

  private:
    // PRIVATE TYPES

    /// This `typedef` is a convenient alias for the implementation-specific
    /// utility class defined in this component.
    typedef ConstructionUtil_Imp Imp;

  public:
    // CLASS METHODS

    /// Create a default-constructed object of (template parameter)
    /// `TARGET_TYPE` at the specified `address`.  If `allocator` is a
    /// `bslma`-compatible allocator and `TARGET_TYPE` supports
    /// `bslma`-style allocation, `allocator` is passed to the default
    /// extended constructor; otherwise, `allocator` is ignored.  If the
    /// constructor throws, the memory at `address` is left in an
    /// unspecified state.  The behavior is undefined unless `address`
    /// refers to a block that is of sufficient size and properly aligned
    /// for objects of `TARGET_TYPE`.
    template <class TARGET_TYPE, class ALLOCATOR>
    static void construct(TARGET_TYPE *address, const ALLOCATOR& allocator);

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13
# ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // 'ARG1' lvalue overloaded unneeded in C++11 and hits bug in gcc < 10.2.
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static void construct(TARGET_TYPE             *address,
                          const ALLOCATOR&         allocator,
                          ARG1&                    argument1,
                          ARGS&&...                arguments);
# endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    /// Create an object of (template parameter) `TARGET_TYPE` at the
    /// specified `address`, constructed by forwarding the specified
    /// `argument1` and the (variable number of) additional specified
    /// `arguments` to the corresponding constructor of `TARGET_TYPE`.  If
    /// the specified `allocator` is a bslma-compatible allocator and
    /// `TARGET_TYPE` supports `bslma`-style allocation, the allocator is
    /// passed to the constructor; otherwise, `allocator` is ignored.  If
    /// the constructor throws, the memory at `address` is left in an
    /// unspecified state.  Note that, in C++03, perfect forwarding is
    /// limited such that any lvalue reference in the `arguments` parameter
    /// pack is const-qualified when forwarded to the `TARGET_TYPE`
    /// constructor; only `argument1` can be forwarded as an unqualified
    /// lvalue.  The behavior is undefined unless `address` refers to a
    /// block that is of sufficient size and properly aligned for objects of
    /// `TARGET_TYPE`.
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static void construct(TARGET_TYPE                             *address,
                          const ALLOCATOR&                         allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  argument1,
                          ARGS&&...                                arguments);
#endif

    /// Create an object of (template parameter) `TARGET_TYPE` at the
    /// specified `address` by moving from the specified `original` object,
    /// then destroy `original`.  The specified `allocator` is unused
    /// (except possibly in precondition checks).  The constructed object
    /// will have the same allocator (if any) as `original`.  If
    /// `bslmf::IsBitwiseMoveable<TARGET_TYPE>::value` is `true`, then the
    /// entire operation is a simple `memcpy` -- no constructors or
    /// destructors are invoked; otherwise, this method move-constructs an
    /// object at `address` from the object at `original` then invokes the
    /// destructor on `original`.  If the move constructor throws an
    /// exception, the memory at `address` is left in an uninitialized state
    /// and `original` is left in a valid but unspecified state.  The
    /// behavior is undefined if `original` uses an allocator other than
    /// `allocator` to supply memory.  Note that if `original` points to an
    /// object of a type derived from `TARGET_TYPE` (i.e., a slicing move)
    /// where `TARGET_TYPE` has a non-`virtual` destructor, then `original`
    /// will be only partially destroyed.
    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(TARGET_TYPE      *address,
                                const ALLOCATOR&  allocator,
                                TARGET_TYPE      *original);

#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
    /// Return, by value, an object of the specified (template parameter)
    /// `TARGET_TYPE`, having default value.  If `allocator` is a
    /// `bslma`-compatible allocator and `TARGET_TYPE` supports
    /// `bslma`-style allocation, `allocator` is passed to the extended
    /// default constructor; otherwise, `allocator` is ignored.  Note that
    /// this method is available only for compilers that reliably implement
    /// copy/move elision (i.e., RVO) on the returned object.  This
    /// copy/move elision is required starting with C++17 and is widely
    /// implemented, though optional, prior to C++17.
    template <class TARGET_TYPE, class ALLOCATOR>
    static TARGET_TYPE make(const ALLOCATOR& allocator);

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
# ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // 'ARG1' lvalue overloaded unneeded in C++11 and hits bug in gcc < 10.2.
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static TARGET_TYPE make(const ALLOCATOR&         allocator,
                            ARG1&                    argument1,
                            ARGS&&...                arguments);
# endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    /// Return, by value, an object of the specified (template parameter)
    /// `TARGET_TYPE`, constructed by forwarding the specified `argument1`
    /// and the (variable number of) additional specified `arguments` to the
    /// corresponding constructor of `TARGET_TYPE`.  If the specified
    /// `allocator` is a bslma-compatible allocator and `TARGET_TYPE`
    /// supports `bslma`-style allocation, the allocator is passed to the
    /// constructor; otherwise, `allocator` is ignored.  Note that this
    /// method is available only for compilers that reliably implement
    /// copy/move elision (i.e., RVO) on the returned object.  This
    /// copy/move elision is required starting with C++17 and is widely
    /// implemented, though optional, prior to C++17.  Note that, in C++03,
    /// perfect forwarding is limited such that any lvalue reference in the
    /// `arguments` parameter pack is const-qualified when forwarded to the
    /// `TARGET_TYPE` constructor; only `argument1` can be forwarded as an
    /// unqualified lvalue.
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static TARGET_TYPE make(const ALLOCATOR&                        allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
                            ARGS&&...                               arguments);
#endif
#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
};

                        // ===========================
                        // struct ConstructionUtil_Imp
                        // ===========================

/// This `struct` provides a namespace for a suite of utility functions that
/// are used to implement functions in `ConstructionUtil`.  In particular,
/// they provide overloads, resolved at compile-time, for various features
/// (e.g., passing down the allocator to sub-elements of `pair`-like types)
/// and optimizations (e.g., bypassing the call to the constructor for
/// classes with trivial default and copy constructors).  These functions
/// should not be used outside this component.
struct ConstructionUtil_Imp {

    // TYPES
    enum {
        // These constants are used in the overloads below, when the last
        // argument is of type 'bsl::integral_constant<int, N> *', indicating
        // that 'TARGET_TYPE' has the traits for which the enumerator equal to
        // 'N' is named.

        e_NIL_TRAITS                 ,
        e_BITWISE_MOVABLE_TRAITS     ,
        e_USES_ALLOCATOR_TRAITS      ,
        e_USES_ALLOCATOR_ARG_T_TRAITS   // Implies USES_ALLOCATOR
    };

    // CLASS METHODS

    /// Construct a default instance of (template parameter) `TARGET_TYPE`
    /// at the specified `address`, passing to the constructor the specified
    /// `allocator` using the leading or trailing allocator convention,
    /// according to the specified `integral_constant` tag, or ignoring
    /// `allocator` in the `e_NIL_TRAITS` case.  If the constructor throws,
    /// the memory at `address` is left in an unspecified state.
    template <class TARGET_TYPE, class ALLOCATOR>
    static void construct(
             TARGET_TYPE      *address,
             const ALLOCATOR&  allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS>);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void construct(
             TARGET_TYPE      *address,
             const ALLOCATOR&  allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_TRAITS>);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void construct(
             TARGET_TYPE      *address,
             const ALLOCATOR&  allocator,
             bsl::integral_constant<int, e_NIL_TRAITS>);

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
# ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // 'ARG1' lvalue overloaded unneeded in C++11 and hits bug in gcc < 10.2.
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static void construct(
             TARGET_TYPE             *address,
             const ALLOCATOR&         allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS>,
             ARG1&                    argument1,
             ARGS&&...                arguments);
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static void construct(
             TARGET_TYPE             *address,
             const ALLOCATOR&         allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_TRAITS>,
             ARG1&                    argument1,
             ARGS&&...                arguments);
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static void construct(
             TARGET_TYPE             *address,
             const ALLOCATOR&         allocator,
             bsl::integral_constant<int, e_NIL_TRAITS>,
             ARG1&                    argument1,
             ARGS&&...                arguments);
# endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    /// Construct an instance of (template parameter) `TARGET_TYPE` at the
    /// specified `address` forwarding to the constructor the specified
    /// `argument1` and `arguments` arguments and passing the specified
    /// `allocator` using the leading or trailing allocator convention,
    /// according to the specified `integral_constant` tag, or ignoring
    /// `allocator` in the `e_NIL_TRAITS` case.  If the constructor throws,
    /// the memory at `address` is left in an unspecified state.
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static void construct(
             TARGET_TYPE                            *address,
             const ALLOCATOR&                        allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS>,
             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
             ARGS&&...                               arguments);
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static void construct(
             TARGET_TYPE                            *address,
             const ALLOCATOR&                        allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_TRAITS>,
             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
             ARGS&&...                               arguments);
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static void construct(
             TARGET_TYPE                            *address,
             const ALLOCATOR&                        allocator,
             bsl::integral_constant<int, e_NIL_TRAITS>,
             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
             ARGS&&...                               arguments);
#endif

    /// Move the bitwise movable object of (template parameter)
    /// `TARGET_TYPE` at the specified `original` address to the specified
    /// `address`, eliding the call to the move constructor and destructor
    /// in favor of performing a bitwise copy.  The specified `allocator`
    /// argument is ignored (except possibly for precondition checks).  The
    /// behavior is undefined if `original` uses an allocator other than
    /// `allocator` to supply memory.
    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(
              TARGET_TYPE                                           *address,
              const ALLOCATOR&                                       allocator,
              bsl::integral_constant<int, e_BITWISE_MOVABLE_TRAITS>  ,
              TARGET_TYPE                                           *original);

    /// Create an object of (template parameter) `TARGET_TYPE` at the
    /// specified `address` by move construction from the specified
    /// `original` object, then destroy `original`.  The specified
    /// `allocator` is unused (except possibly in precondition checks).  The
    /// constructed object will have the same allocator (if any) as
    /// `original`.  If the move constructor throws an exception, the memory
    /// at `address` is left in an uninitialized state and `original` is
    /// left in a valid but unspecified state.  The behavior is undefined if
    /// `original` uses an allocator other than `allocator` to supply
    /// memory.  Note that, if `original` points to an object of a type
    /// derived from `TARGET_TYPE` (i.e., a slicing move) where
    /// `TARGET_TYPE` has a non-`virtual` destructor, then `original` will
    /// be only partially destroyed.
    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(
                          TARGET_TYPE                               *address,
                          const ALLOCATOR&                           allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS>  ,
                          TARGET_TYPE                               *original);

#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
    /// Return, by value, a default-constructed object of (template
    /// parameter) `TARGET_TYPE`, passing the specified `allocator` to the
    /// constructor using the leading or trailing allocator convention,
    /// according to the specified `integral_constant` tag, or ignoring
    /// `allocator` in the `e_NIL_TRAITS` case.
    template <class TARGET_TYPE, class ALLOCATOR>
    static TARGET_TYPE make(
             const ALLOCATOR&  allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS>);
    template <class TARGET_TYPE, class ALLOCATOR>
    static TARGET_TYPE make(
             const ALLOCATOR&  allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_TRAITS>);
    template <class TARGET_TYPE, class ALLOCATOR>
    static TARGET_TYPE make(
             const ALLOCATOR&  allocator,
             bsl::integral_constant<int, e_NIL_TRAITS>);

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
# ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // 'ARG1' lvalue overloaded unneeded in C++11 and hits bug in gcc < 10.2.
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static TARGET_TYPE make(
             const ALLOCATOR&         allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS>,
             ARG1&                    argument1,
             ARGS&&...                arguments);
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static TARGET_TYPE make(
             const ALLOCATOR&         allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_TRAITS>,
             ARG1&                    argument1,
             ARGS&&...                arguments);
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static TARGET_TYPE make(
             const ALLOCATOR&         allocator,
             bsl::integral_constant<int, e_NIL_TRAITS>,
             ARG1&                    argument1,
             ARGS&&...                arguments);
# endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    /// Return, by value, an instance of (template parameter) `TARGET_TYPE`,
    /// forwarding to the constructor the specified `argument1` and
    /// `arguments` arguments and passing the specified `allocator` using
    /// the leading or trailing allocator convention, according to the
    /// specified `integral_constant` tag, or ignoring `allocator` in the
    /// `e_NIL_TRAITS` case.
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static TARGET_TYPE make(
             const ALLOCATOR&                        allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS>,
             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
             ARGS&&...                               arguments);
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static TARGET_TYPE make(
             const ALLOCATOR&                        allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_TRAITS>,
             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
             ARGS&&...                               arguments);
    template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
    static TARGET_TYPE make(
             const ALLOCATOR&                        allocator,
             bsl::integral_constant<int, e_NIL_TRAITS>,
             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
             ARGS&&...                               arguments);

#endif
#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)

    /// Return the specified `address` cast as a pointer to `void`, even if
    /// the (template parameter) `TARGET_TYPE` is cv-qualified.
    template <class TARGET_TYPE>
    static void *voidify(TARGET_TYPE *address);
};

// ============================================================================
//                      TEMPLATE IMPLEMENTATIONS
// ============================================================================

              // -----------------------------------------------
              // struct template ConstructionUtil_IsAllocatorPtr
              // -----------------------------------------------

/// Metafunction that inherits from `true_type` if the `ALLOC` type is a
/// pointer to `bslma::Allocator` or a class derived from it and
/// `false_type` otherwise.
template <class ALLOC>
struct ConstructionUtil_IsAllocatorPtr;

/// This primary template is instantiated only for non-pointer type `ALLOC`
/// and always evaluates false.
template <class ALLOC>
struct ConstructionUtil_IsAllocatorPtr : bsl::false_type {
};

/// This partial specialization is for pointer type `ALLOC *` and evaulates
/// true if and only if `ALLOC` is derived from `bslma::Allocator`.
template <class ALLOC>
struct ConstructionUtil_IsAllocatorPtr<ALLOC *>
    : bsl::is_convertible<ALLOC *, bslma::Allocator *>::type {
};

              // ---------------------------------------------------
              // struct template ConstructionUtil_ConstructionTraits
              // ---------------------------------------------------

/// Metafunction yielding one of the following three `value` constants:
///
///: e_USES_ALLOCATOR_ARG_T_TRAITS  `TARGET_TYPE` supports an allocator as
///:                                its second constructor argument, after
///:                                `bsl::allocator_arg`.
///:
///: e_USES_ALLOCATOR_TRAITS        `TARGET_TYPE` supports an allocator as
///:                                its last constructor argument.
///:
///: e_NIL_TRAITS                   `TARGET_TYPE` does not support an
///:                                allocator and/or `ALLOCATOR` is `void*`
///:                                `int`, or any other non-allocator
///:                                scalar.  The allocator argument to
///:                                `construct` will be ignored.
///
/// `ALLOCATOR` is compatible with an AA `TARGET_TYPE` if `ALLOCATOR` is
/// convertible to the allocator type expected by `TARGET_TYPE`'s
/// constructors.  There are three special cases of incompatible `ALLOCATOR`
/// types:
///
/// 1. If `TARGET_TYPE` expects an allocator of type `bslma::Allocator *`
///    this metafunction yields either `e_USES_ALLOCATOR_ARG_T_TRAITS` or
///    `e_USES_ALLOCATOR_TRAITS`, even if `ALLOCATOR` is not compatible with
///    `TARGET_TYPE`.  The `construct` method will attempt to extract the
///    `bslma::Allocator *` from the allocator using its `mechanism`
///    accessor.  If there is no `mechanism` accessor, compilation will
///    fail.
/// 2. Otherwise, if `bslma::UsesBslmaAllocator<TARGET_TYPE>::value` is
///    `true`, this metafunction yields either
///    `e_USES_ALLOCATOR_ARG_T_TRAITS` or `e_USES_ALLOCATOR_TRAITS`, even if
///    `ALLOCATOR` is not compatible with `TARGET_TYPE`, resulting in a
///    compilation error within `construct`.
/// 3. Otherwise, if `ALLOCATOR` is incompatible with `TARGET_TYPE`, then
///    `ALLOCATOR` is ignored and this metafunction yields `e_NIL_TRAITS`,
///    as though `TARGET_TYPE` were not AA.
///
/// The first two special cases allow mixing and matching between
/// `bsl::allocator` and `bslma::Allocator *` in both directions, but will
/// fail for other allocator types.  The failure is desirable to prevent
/// code accidentally passing an incorrect allocator type.
///
/// The third special case is inconsistent with the other two so as to allow
/// third-party classes that use third-party allocators to be treated as
/// non-AA within containers that use BDE allocators.
///
/// This metafunction also yields a `type` of 'bsl::integral_constant<value,
/// int>'.
template <class TARGET_TYPE, class ALLOCATOR>
struct ConstructionUtil_ConstructionTrait {

  private:
    // PRIVATE TYPES
    typedef ConstructionUtil_Imp Imp;

  public:
    // PUBLIC TYPES
    enum {
        value = ((UsesBslmaAllocator<TARGET_TYPE>::value ||
                  bsl::uses_allocator<TARGET_TYPE, ALLOCATOR>::value) &&
                 (bsl::is_class<ALLOCATOR>::value ||
                  ConstructionUtil_IsAllocatorPtr<ALLOCATOR>::value))
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                   ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                   : Imp::e_USES_ALLOCATOR_TRAITS)
                : Imp::e_NIL_TRAITS
    };

    typedef bsl::integral_constant<int, value> type;
};


              // -------------------------------------------------
              // struct template ConstructionUtil_AllocAdaptorUtil
              // -------------------------------------------------

template <class TYPE,
          bool HAS_ALLOC_TYPE = HasAllocatorType<TYPE>::value,
          bool IS_BSLMA_AA    = UsesBslmaAllocator<TYPE>::value>
struct ConstructionUtil_AllocAdaptorUtil;

/// This utility class template provides a static `adapt` method that adapts
/// an allocator object to the type expected by `TYPE`.  This primary
/// template is instantiated only for a non-AA `TYPE`; such types do not
/// expect an allocator, so no `adapt` method is defined.
template <class TYPE>
struct ConstructionUtil_AllocAdaptorUtil<TYPE, false, false> {
};

/// This utility class template provides a static `adapt` method that adapts
/// an allocator object to the type expected by `TYPE`.  This partial
/// specialization is instantiated for types that expect an allocator of
/// type `bslma::Allocator *`.
template <class TYPE>
struct ConstructionUtil_AllocAdaptorUtil<TYPE, false, true> {

    /// Return the `bslma::Allocator` pointer held by the specified `a`
    /// object of non-pointer class.  Compilation will fail if type `ALLOC`
    /// does not provide a `mechanism()` accessor returning a pointer to
    /// type convertible to `bslma::Allocator *`.
    template <class ALLOC>
    static bslma::Allocator *adapt(const ALLOC& a) { return a.mechanism(); }

    /// Return the specified `alloc_p` pointer, implicitly converted to
    /// `bslma::Allocator *`.  Compilation will fail unless `ALLOC` is
    /// derived from `bslma::Allocator`.
    template <class ALLOC>
    static bslma::Allocator *adapt(ALLOC *const &alloc_p) { return alloc_p; }
};

/// This utility class template provides a static `adapt` method that adapts
/// an allocator object to the type expected by `TYPE`.  This partial
/// specialization is instantiated for types that expect an allocator of
/// type `TYPE::allocator_type`.
template <class TYPE, bool IS_BSLMA_AA>
struct ConstructionUtil_AllocAdaptorUtil<TYPE, true, IS_BSLMA_AA> {

    /// Return the specified `a` allocator, implicitly converted to
    /// `TYPE::allocator_type`.  Compilation will fail if implicit
    /// conversion to the return type is invalid.
    template <class ALLOC>
    static typename TYPE::allocator_type adapt(const ALLOC& a) { return a; }
};


                          // -----------------------
                          // struct ConstructionUtil
                          // -----------------------

// CLASS METHODS
template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            const ALLOCATOR&  allocator)
{
    typedef typename
        ConstructionUtil_ConstructionTrait<TARGET_TYPE, ALLOCATOR>::type Trait;

    Imp::construct(address, allocator, Trait());
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
# ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
void
ConstructionUtil::construct(TARGET_TYPE             *address,
                            const ALLOCATOR&         allocator,
                            ARG1&                    argument1,
                            ARGS&&...                arguments)
{
    typedef typename
        ConstructionUtil_ConstructionTrait<TARGET_TYPE, ALLOCATOR>::type Trait;

    Imp::construct(address,
                   allocator,
                   Trait(),
                   argument1,
                   BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
}
# endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
void
ConstructionUtil::construct(TARGET_TYPE                            *address,
                            const ALLOCATOR&                        allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
                            ARGS&&...                               arguments)
{
    typedef typename
        ConstructionUtil_ConstructionTrait<TARGET_TYPE, ALLOCATOR>::type Trait;

    Imp::construct(address,
                   allocator,
                   Trait(),
                   BSLS_COMPILERFEATURES_FORWARD(ARG1, argument1),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
}
#endif

template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil::destructiveMove(TARGET_TYPE      *address,
                                  const ALLOCATOR&  allocator,
                                  TARGET_TYPE      *original)
{
    BSLS_ASSERT_SAFE(address);
    BSLS_ASSERT_SAFE(original);

    enum {
        k_VALUE = bslmf::IsBitwiseMoveable<TARGET_TYPE>::value
                ? Imp::e_BITWISE_MOVABLE_TRAITS
                : Imp::e_NIL_TRAITS
    };

    Imp::destructiveMove(address,
                         allocator,
                         bsl::integral_constant<int, k_VALUE>(),
                         original);
}

#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
// Suppress bde_verify warnings about return-by-value in this region.
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -AR01: Type using allocator is returned by value

template <class TARGET_TYPE, class ALLOCATOR>
inline
TARGET_TYPE
ConstructionUtil::make(const ALLOCATOR&  allocator)
{
    typedef typename
        ConstructionUtil_ConstructionTrait<TARGET_TYPE, ALLOCATOR>::type Trait;

    return Imp::make<TARGET_TYPE>(allocator, Trait());
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
# ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
TARGET_TYPE
ConstructionUtil::make(const ALLOCATOR&         allocator,
                       ARG1&                    argument1,
                       ARGS&&...                arguments)
{
    typedef typename
        ConstructionUtil_ConstructionTrait<TARGET_TYPE, ALLOCATOR>::type Trait;

    return Imp::make<TARGET_TYPE>(
                          allocator,
                          Trait(),
                          argument1,
                          BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
}
# endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
TARGET_TYPE
ConstructionUtil::make(const ALLOCATOR&                        allocator,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
                       ARGS&&...                               arguments)
{
    typedef typename
        ConstructionUtil_ConstructionTrait<TARGET_TYPE, ALLOCATOR>::type Trait;

    return Imp::make<TARGET_TYPE>(
                          allocator,
                          Trait(),
                          BSLS_COMPILERFEATURES_FORWARD(ARG1, argument1),
                          BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
}
#endif

// BDE_VERIFY pragma: pop
#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)

                       // ---------------------------
                       // struct ConstructionUtil_Imp
                       // ---------------------------

// CLASS METHODS
template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil_Imp::construct(
             TARGET_TYPE      *address,
             const ALLOCATOR&  allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS>)
{
    typedef ConstructionUtil_AllocAdaptorUtil<TARGET_TYPE> AllocUtil;
    ::new (voidify(address)) TARGET_TYPE(bsl::allocator_arg,
                                         AllocUtil::adapt(allocator));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil_Imp::construct(
             TARGET_TYPE      *address,
             const ALLOCATOR&  allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_TRAITS>)
{
    typedef ConstructionUtil_AllocAdaptorUtil<TARGET_TYPE> AllocUtil;
    ::new (voidify(address)) TARGET_TYPE(AllocUtil::adapt(allocator));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil_Imp::construct(
             TARGET_TYPE      *address,
             const ALLOCATOR&  ,
             bsl::integral_constant<int, e_NIL_TRAITS>)
{
    ::new (voidify(address)) TARGET_TYPE();
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
# ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
void
ConstructionUtil_Imp::construct(
         TARGET_TYPE             *address,
         const ALLOCATOR&         allocator,
         bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS>,
         ARG1&                    argument1,
         ARGS&&...                arguments)
{
    typedef ConstructionUtil_AllocAdaptorUtil<TARGET_TYPE> AllocUtil;
    ::new (voidify(address)) TARGET_TYPE(
        bsl::allocator_arg,
        AllocUtil::adapt(allocator),
        argument1,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
void
ConstructionUtil_Imp::construct(
         TARGET_TYPE             *address,
         const ALLOCATOR&         allocator,
         bsl::integral_constant<int, e_USES_ALLOCATOR_TRAITS>,
         ARG1&                    argument1,
         ARGS&&...                arguments)
{
    typedef ConstructionUtil_AllocAdaptorUtil<TARGET_TYPE> AllocUtil;
    ::new (voidify(address)) TARGET_TYPE(
        argument1,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...,
        AllocUtil::adapt(allocator));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
void
ConstructionUtil_Imp::construct(
         TARGET_TYPE             *address,
         const ALLOCATOR&         ,
         bsl::integral_constant<int, e_NIL_TRAITS>,
         ARG1&                    argument1,
         ARGS&&...                arguments)
{
    ::new (voidify(address)) TARGET_TYPE(
        argument1,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}
# endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
void
ConstructionUtil_Imp::construct(
         TARGET_TYPE                            *address,
         const ALLOCATOR&                        allocator,
         bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS>,
         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
         ARGS&&...                               arguments)
{
    typedef ConstructionUtil_AllocAdaptorUtil<TARGET_TYPE> AllocUtil;
    ::new (voidify(address)) TARGET_TYPE(
        bsl::allocator_arg,
        AllocUtil::adapt(allocator),
        BSLS_COMPILERFEATURES_FORWARD(ARG1, argument1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
void
ConstructionUtil_Imp::construct(
         TARGET_TYPE                            *address,
         const ALLOCATOR&                        allocator,
         bsl::integral_constant<int, e_USES_ALLOCATOR_TRAITS>,
         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
         ARGS&&...                               arguments)
{
    typedef ConstructionUtil_AllocAdaptorUtil<TARGET_TYPE> AllocUtil;
    ::new (voidify(address)) TARGET_TYPE(
        BSLS_COMPILERFEATURES_FORWARD(ARG1, argument1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...,
        AllocUtil::adapt(allocator));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
void
ConstructionUtil_Imp::construct(
         TARGET_TYPE                            *address,
         const ALLOCATOR&                        ,
         bsl::integral_constant<int, e_NIL_TRAITS>,
         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
         ARGS&&...                               arguments)
{
    ::new (voidify(address)) TARGET_TYPE(
        BSLS_COMPILERFEATURES_FORWARD(ARG1, argument1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}
#endif

template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil_Imp::destructiveMove(
               TARGET_TYPE                                           *address,
               const ALLOCATOR&                                       ,
               bsl::integral_constant<int, e_BITWISE_MOVABLE_TRAITS>  ,
               TARGET_TYPE                                           *original)
{
    if (bsl::is_fundamental<TARGET_TYPE>::value ||
        bsl::is_pointer<TARGET_TYPE>::value) {
        ::new (voidify(address)) TARGET_TYPE(*original);
        BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
    }
    else {
        // voidify(address) is used here to suppress compiler warning
        // "-Wclass-memaccess".
        memcpy(voidify(address), original, sizeof *original);
    }
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil_Imp::destructiveMove(
                          TARGET_TYPE                               *address,
                          const ALLOCATOR&                           allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS>  ,
                          TARGET_TYPE                               *original)
{
    // TBD: Eventually, we can add a precondition that 'allocator' matches
    // 'original''s allocator, but that is not universally detectable right
    // now, as not all allocator-aware types provide an 'allocator()' method.
    //..
    // BSLS_ASSERT(allocator == original->allocator());

    ConstructionUtil::construct(address,
                                allocator,
                                bslmf::MovableRefUtil::move(*original));
    DestructionUtil::destroy(original);
}

#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
// Suppress bde_verify warnings about return-by-value in this region.
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -AR01: Type using allocator is returned by value

template <class TARGET_TYPE, class ALLOCATOR>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
             const ALLOCATOR&  allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS>)
{
    typedef ConstructionUtil_AllocAdaptorUtil<TARGET_TYPE> AllocUtil;
    return TARGET_TYPE(bsl::allocator_arg, AllocUtil::adapt(allocator));
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
             const ALLOCATOR&  allocator,
             bsl::integral_constant<int, e_USES_ALLOCATOR_TRAITS>)
{
    typedef ConstructionUtil_AllocAdaptorUtil<TARGET_TYPE> AllocUtil;
    return TARGET_TYPE(AllocUtil::adapt(allocator));
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
             const ALLOCATOR&  ,
             bsl::integral_constant<int, e_NIL_TRAITS>)
{
    return TARGET_TYPE();
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
# ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
         const ALLOCATOR&         allocator,
         bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS>,
         ARG1&                    argument1,
         ARGS&&...                arguments)
{
    typedef ConstructionUtil_AllocAdaptorUtil<TARGET_TYPE> AllocUtil;
    return TARGET_TYPE(
        bsl::allocator_arg,
        AllocUtil::adapt(allocator),
        argument1,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
}

template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
         const ALLOCATOR&         allocator,
         bsl::integral_constant<int, e_USES_ALLOCATOR_TRAITS>,
         ARG1&                    argument1,
         ARGS&&...                arguments)
{
    typedef ConstructionUtil_AllocAdaptorUtil<TARGET_TYPE> AllocUtil;
    return TARGET_TYPE(
        argument1,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...,
        AllocUtil::adapt(allocator));
}

template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
         const ALLOCATOR&         ,
         bsl::integral_constant<int, e_NIL_TRAITS>,
         ARG1&                    argument1,
         ARGS&&...                arguments)
{
    return TARGET_TYPE(
        argument1,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
}
# endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
         const ALLOCATOR&                        allocator,
         bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS>,
         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
         ARGS&&...                               arguments)
{
    typedef ConstructionUtil_AllocAdaptorUtil<TARGET_TYPE> AllocUtil;
    return TARGET_TYPE(
        bsl::allocator_arg,
        AllocUtil::adapt(allocator),
        BSLS_COMPILERFEATURES_FORWARD(ARG1, argument1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
}

template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
         const ALLOCATOR&                        allocator,
         bsl::integral_constant<int, e_USES_ALLOCATOR_TRAITS>,
         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
         ARGS&&...                               arguments)
{
    typedef ConstructionUtil_AllocAdaptorUtil<TARGET_TYPE> AllocUtil;
    return TARGET_TYPE(
        BSLS_COMPILERFEATURES_FORWARD(ARG1, argument1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...,
        AllocUtil::adapt(allocator));
}

template <class TARGET_TYPE, class ALLOCATOR, class ARG1, class... ARGS>
inline
TARGET_TYPE
ConstructionUtil_Imp::make(
         const ALLOCATOR&                        ,
         bsl::integral_constant<int, e_NIL_TRAITS>,
         BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) argument1,
         ARGS&&...                               arguments)
{
    return TARGET_TYPE(
        BSLS_COMPILERFEATURES_FORWARD(ARG1, argument1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
}
#endif

// BDE_VERIFY pragma: pop
#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)

template <class TARGET_TYPE>
inline
void *ConstructionUtil_Imp::voidify(TARGET_TYPE *address)
{
    return static_cast<void *>(
            const_cast<typename bsl::remove_cv<TARGET_TYPE>::type *>(address));
}

}  // close package namespace
}  // close enterprise namespace

#endif // End C++11 code

#endif

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
