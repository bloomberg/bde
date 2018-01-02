// bsls_atomicoperations_default.h                                    -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#define INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide default implementation for atomic operations.
//
//@CLASSES:
//  bsls::AtomicOperations_DefaultInt: defaults for atomic operations on int
//  bsls::AtomicOperations_DefaultInt64: defaults for Int64
//  bsls::AtomicOperations_DefaultPointer32: defaults for 32-bit pointer
//  bsls::AtomicOperations_DefaultPointer64: defaults for 64-bit pointer
//  bsls::AtomicOperations_Default32: all atomics for a generic 32-bit platform
//  bsls::AtomicOperations_Default64: all atomics for a generic 64-bit platform
//
//@DESCRIPTION: [!PRIVATE!] This component provides classes having default
// implementations of atomic operations independent of a any specific platform.
// The purpose of these classes is for them to be combined with
// platform-specific implementations of atomic operations to form a full set of
// atomic operations.
//
// [!WARNING!] This component should not be used directly by client code.  It
// is subject to change without warning.
//
// The implementation of atomic operations splits the set of operations into
// two groups: essential (core) operations and non-essential operations.  The
// essential core operations, such as 'getInt', 'setInt', 'swapInt',
// 'testAndSwapInt', must be implemented on every target platform.  The
// non-essential operations, such as 'incrementInt', 'decrementInt', have
// default implementations written in terms of the core operations (though the
// default implementation may be overridden for a target platform).
//
// This component provides the default implementations for non-essential atomic
// operations.  For the essential core operations, it provides only function
// prototypes without any concrete implementation.  The concrete implementation
// for core operations on each platform are provided by a platform-specific
// implementation component.
//
///Design of Components Providing Atomic Operations
///------------------------------------------------
// There are about 70 different atomic operations we provide in this atomics
// library.  Only a fraction of these operations (about 10-15) have
// implementations specific to a target platform.  These are the core atomic
// operations.  Other operations have implementations that are independent of
// the platform, and are defined in terms of those core operations.
//
// A primary design objective for this implementation of atomic operations is
// to provide a single common implementation for the platform-independent,
// non-essential, atomic operations.  This way, each platform-specific
// implementation of atomics needs to provide only the 10-15 core operations,
// and may obtain the remaining 60 atomic operations automatically from this
// default implementation component.
//
// The implementation technique used to achieve this goal is called the
// Curiously Recurring Template Pattern, or CRTP.
//
// In the CRTP a derived class inherits from the base class, but the base class
// is parameterized with the derived class template parameter.  In the context
// of atomic operations, the CRTP allows the non-essential base class methods
// to be accessed through the platform specific derived class, while the
// template parameterization of the base class allows the implementations for
// platform-independent (non-essential) operations in the base class to be
// implemented in terms of the core operations implemented in the
// platform-specific derived class.
//
// Let's illustrate this idea with an example.  The base class, 'AtomicsBase',
// will provide an implementation for a single non-essential atomic operation,
// 'getIntAcquire', whose implementation delegates to the platform-specific
// implementation of another, (core) atomic operation, 'getInt':
//..
//  template <class IMP>
//  struct AtomicsBase<IMP>
//  {
//      // static
//      // int getInt(int volatile *);
//      //   Implemented in the derived class, 'IMP'.
//
//      static
//      int getIntAcquire(int volatile *obj)
//          // Platform-independent; implemented in terms of the derived
//          // 'getInt'.
//      {
//          return IMP::getInt(obj);
//      }
//  };
//..
// Now we define a platform-specific implementation of the atomics class for
// the x86 platform providing the core operation 'getInt':
//..
//  struct AtomicsX86 : AtomicsBase<AtomicsX86>
//  {
//      static
//      int getInt(int volatile *obj)
//          // Platform-specific implementation.
//      {
//          // ...whatever is necessary for this atomic operation on x86
//      }
//  };
//..
// To get an idea what the effective interface and the implementation of the
// 'AtomicsX86' class looks like, we can flatten the hierarchy of 'AtomicsX86'
// and remove the 'AtomicsBase' class from the picture, as if we implemented
// 'AtomicsX86' without the help of 'AtomicsBase'.  The effective interface and
// the implementation of 'AtomicsX86' is as follows:
//..
//  struct AtomicsX86Flattened
//  {
//      static
//      int getInt(int volatile *obj)
//          // Platform-specific implementation.
//      {
//          // ...whatever is necessary for this atomic operation on x86
//      }
//
//      static
//      int getIntAcquire(int volatile *obj)
//          // Platform-independent; implemented in terms of 'getInt'.
//      {
//          return getInt(obj);
//      }
//  };
//..
// The above code shows that the design goal is achieved: Platform-independent
// atomic operations are factored out into easily reusable 'AtomicsBase' which,
// when combined with platform-specific atomic operations, produces the
// complete set of atomic operations for a given target-platform.
//
///Component and Class Hierarchy for Atomic Operations
///---------------------------------------------------
// This section describes the hierarchy of components and types that implement
// atomic operations.  Below is the list of base classes providing default
// implementations of platform-independent, non-essential atomic operations.
// Atomic operations for different data types are factored into their own base
// classes:
//: o bsls::AtomicOperations_DefaultInt - provides atomic operations for int
//: o bsls::AtomicOperations_DefaultInt64 - for Int64
//: o bsls::AtomicOperations_DefaultPointer32 - for 32-bit pointer
//: o bsls::AtomicOperations_DefaultPointer64 - for 64-bit pointer
//
// The platform-specific core atomic operations are left unimplemented in these
// default implementation classes.  The implementations for those operations
// have to be provided by a platform-specific derived classes.
//
// The above default implementation classes are combined (using inheritance)
// into more specialized base classes that provide nearly complete set of
// atomic operations for a generic platform:
//
//: o bsls::AtomicOperations_Default32 - a set of atomic operations for a
//:   generic 32-bit platform
//:
//: o bsls::AtomicOperations_Default64 - a set of atomic operations for a
//:   generic 64-bit platform
//
// This is how the generic platform base classes are composed:
//: o bsls::AtomicOperations_Default32 : AtomicOperations_DefaultInt,
//:   AtomicOperations_DefaultInt64, AtomicOperations_DefaultPointer32
//: o bsls::AtomicOperations_Default64 : AtomicOperations_DefaultInt,
//:   AtomicOperations_DefaultInt64, AtomicOperations_DefaultPointer64
//
// A typical derived class implementing platform-specific atomic operations
// needs to derive from either 'bsls::AtomicOperations_Default32' (if the
// platform is 32-bit) or 'bsls::AtomicOperations_Default64' (if the platform
// is 64-bit).
//
// For example, let's take the X86_64 platform with GCC compiler.  The derived
// class for this platform, 'bsls::AtomicOperations_X64_ALL_GCC', inherits from
// 'bsls::AtomicOperations_Default64' and implements all platform-specific
// atomic operations:
//..
//  struct bsls::AtomicOperations_X64_ALL_GCC
//      : bsls::AtomicOperations_Default64<bsls::AtomicOperations_X64_ALL_GCC>
//  {
//      typedef bsls::Atomic_TypeTraits<bsls::AtomicOperations_X64_ALL_GCC>
//          AtomicTypes;   // for explanation of atomic type traits see below
//
//          // *** atomic functions for int ***
//      static int getInt(const AtomicTypes::Int *atomicInt);
//      static void setInt(AtomicTypes::Int *atomicInt, int value);
//      // ...
//
//          // *** atomic functions for Int64 ***
//      static bsls::Types::Int64 getInt64(
//                  AtomicTypes::Int64 const *atomicInt);
//      static void setInt64(
//                  AtomicTypes::Int64 *atomicInt, bsls::Types::Int64 value);
//      // ...
//  };
//..
// A derived class can also override some default atomic implementations from a
// base class.  For example, 'bsls::AtomicOperations_X64_ALL_GCC' can provide a
// better implementation for 'getIntAcquire' than the one provided by the
// 'bsls::AtomicOperations_DefaultInt' base class.  So
// 'bsls::AtomicOperations_X64_ALL_GCC' implements its own 'getIntAcquire' with
// the same signature as in the base class:
//..
//  struct bsls::AtomicOperations_X64_ALL_GCC
//      : bsls::AtomicOperations_Default64<bsls::AtomicOperations_X64_ALL_GCC>
//  {
//          // *** atomic functions for int ***
//      static int getIntAcquire(const AtomicTypes::Int *atomicInt);
//      // ...
//  };
//..
// Technically speaking, this is not overriding, but hiding the respective
// methods of the base class, but for our purpose, it works as if the methods
// were overridden.
//
// Platform-specific atomic operations for other platforms are implemented in a
// similar manner.  Here is a conceptual diagram of relationships between
// classes that provide the default atomic operations and platform-specific
// atomic operations (the bsls::AtomicOperations_ prefix is omitted for
// brevity):
//..
//  DefaultPointer32      DefaultInt         DefaultInt64    DefaultPointer64
//          ^                  ^                   ^                 ^
//          |                  |                   |                 |
//          +---------------+  +-------------------+  +--------------+
//                          |  |                   |  |
//                        Default32              Default64
//                            ^                      ^
//                            |                      |
//              X86_ALL_GCC --+                      +-- X64_ALL_GCC
//                            |                      |
//             X86_WIN_MSVC --+                      +-- X64_WIN_MSVC
//                            |                      |
//        POWERPC32_AIX_XLC --+                      +-- POWERPC64_AIX_XLC
//                            |                      |
//           SPARC32_SUN_CC --+                      +-- SPARC64_SUN_CC
//                                                   |
//                                                   +-- IA64_HP_ACC
//..
//
// The last part of the implementation is the atomic type traits class.  We
// need a special representation for atomic primitive types to ensure
// conformance to size and alignment requirements for each platform.  For
// example, the atomic primitive type for 'int' is usually a 4-byte aligned
// 'volatile int'.  Since the alignment is specified differently for different
// platforms and compilers, the atomic type traits class allows us to abstract
// the rest of the implementation of atomics from those differences.
//
// This default implementation component provides only a declaration of a
// generic atomic type traits class:
//..
//  template <class IMP>
//  struct bsls::Atomic_TypeTraits;
//..
// Each platform-specific implementation has to provide its own specialization
// of the type traits class.  For example, the specialization for the x86_64
// platform defined by the 'bsls::AtomicOperations_X64_ALL_GCC' class might
// look like:
//..
//  template <>
//  struct bsls::Atomic_TypeTraits<bsls::AtomicOperations_X64_ALL_GCC>
//  {
//      struct Int
//      {
//          volatile int d_value __attribute__((__aligned__(sizeof(int))));
//      };
//
//      struct Int64
//      {
//          volatile bsls::Types::Int64 d_value
//                    __attribute__((__aligned__(sizeof(bsls::Types::Int64))));
//      };
//
//      struct Pointer
//      {
//          void * volatile d_value
//                                __attribute__((__aligned__(sizeof(void *))));
//      };
//  };
//..
//
///Usage
///-----
// This component is a private implementation type of 'bsls_atomicoperations';
// see 'bsls_atomicoperations' for a usage example.

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

namespace bsls {

                     // ==================================
                     // struct AtomicOperations_DefaultInt
                     // ==================================

template <class IMP>
struct Atomic_TypeTraits;

template <class IMP>
struct AtomicOperations_DefaultInt
    // This class provides default implementations of non-essential atomic
    // operations for the 32-bit integer type independent on any specific
    // platform.  It also provides prototypes for the atomic operations for the
    // 32-bit integer type that have to be implemented separately for each
    // specific platform.  These platform-independent and platform-specific
    // atomic operations together form a full set of atomic operations for the
    // 32-bit integer type.
{
  public:
    // PUBLIC TYPES
    typedef Atomic_TypeTraits<IMP> AtomicTypes;

  private:
    // The following are signatures for the core atomics interface, the
    // implementation of which must be provided by a derived platform-specific
    // 'IMP' class.

    // NOT IMPLEMENTED
    static int addIntNv(typename AtomicTypes::Int *atomicInt, int value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, providing the sequential consistency
        // memory ordering guarantee.

    static int getInt(typename AtomicTypes::Int const *atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt',
        // providing the sequential consistency memory ordering guarantee.

    static void setInt(typename AtomicTypes::Int *atomicInt, int value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', providing the sequential consistency memory
        // ordering guarantee.

    static int swapInt(typename AtomicTypes::Int *atomicInt, int swapValue);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'swapValue', and return its previous value, providing the
        // sequential consistency memory ordering guarantee.

    static int testAndSwapInt(typename AtomicTypes::Int *atomicInt,
                              int                        compareValue,
                              int                        swapValue);
        // Conditionally set the value of the specified 'atomicInt' to the
        // specified 'swapValue' if and only if the value of 'atomicInt' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicInt', providing the sequential consistency memory
        // ordering guarantee.  The whole operation is performed atomically.

  public:
    // CLASS METHODS
    static int getIntAcquire(typename AtomicTypes::Int const *atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt',
        // providing the acquire memory ordering guarantee.

    static int getIntRelaxed(typename AtomicTypes::Int const *atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt', without
        // providing any memory ordering guarantees.

    static void initInt(typename AtomicTypes::Int *atomicInt,
                        int                        initialValue = 0);
        // Initialize the specified 'atomicInt' and set its value to the
        // optionally specified 'initialValue'.

    static void setIntRelaxed(typename AtomicTypes::Int *atomicInt, int value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', without providing any memory ordering guarantees.

    static void setIntRelease(typename AtomicTypes::Int *atomicInt, int value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', providing the release memory ordering guarantee.

    static int swapIntAcqRel(typename AtomicTypes::Int *atomicInt,
                             int                        swapValue);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'swapValue', and return its previous value, providing the
        // acquire/release memory ordering guarantee.

    static int testAndSwapIntAcqRel(typename AtomicTypes::Int *atomicInt,
                                    int                        compareValue,
                                    int                        swapValue);
        // Conditionally set the value of the specified 'atomicInt' to the
        // specified 'swapValue' if and only if the value of 'atomicInt' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicInt', providing the acquire/release memory ordering
        // guarantee.  The whole operation is performed atomically.

    // Arithmetic
    static void addInt(typename AtomicTypes::Int *atomicInt, int value);
        // Atomically add to the specified 'atomicInt' the specified 'value',
        // providing the sequential consistency memory ordering guarantee.

    static void addIntAcqRel(typename AtomicTypes::Int *atomicInt, int value);
        // Atomically add to the specified 'atomicInt' the specified 'value',
        // providing the acquire/release memory ordering guarantee.

    static void addIntRelaxed(typename AtomicTypes::Int *atomicInt, int value);
        // Atomically add to the specified 'atomicInt' the specified 'value',
        // without providing any memory ordering guarantees.

    static int addIntNvAcqRel(typename AtomicTypes::Int *atomicInt, int value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, providing the acquire/release memory
        // ordering guarantee.

    static int addIntNvRelaxed(typename AtomicTypes::Int *atomicInt,
                               int                        value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, without providing any memory
        // ordering guarantees.

    static void decrementInt(typename AtomicTypes::Int *atomicInt);
        // Atomically decrement the value of the specified 'atomicInt' by 1,
        // providing the sequential consistency memory ordering guarantee.

    static void decrementIntAcqRel(typename AtomicTypes::Int *atomicInt);
        // Atomically decrement the value of the specified 'atomicInt' by 1,
        // providing the acquire/release memory ordering guarantee.

    static int decrementIntNv(typename AtomicTypes::Int *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1 and return the
        // resulting value, providing the sequential consistency memory
        // ordering guarantee.

    static int decrementIntNvAcqRel(typename AtomicTypes::Int *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1 and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    static void incrementInt(typename AtomicTypes::Int *atomicInt);
        // Atomically increment the value of the specified 'atomicInt' by 1,
        // providing the sequential consistency memory ordering guarantee.

    static void incrementIntAcqRel(typename AtomicTypes::Int *atomicInt);
        // Atomically increment the value of the specified 'atomicInt' by 1,
        // providing the acquire/release memory ordering guarantee.

    static int incrementIntNv(typename AtomicTypes::Int *atomicInt);
        // Atomically increment the specified 'atomicInt' by 1 and return the
        // resulting value, providing the sequential consistency memory
        // ordering guarantee.

    static int incrementIntNvAcqRel(typename AtomicTypes::Int *atomicInt);
        // Atomically increment the specified 'atomicInt' by 1 and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    static int subtractIntNv(typename AtomicTypes::Int *atomicInt, int value);
        // Atomically subtract from the specified 'atomicInt' the specified
        // 'value' and return the resulting value, providing the sequential
        // consistency memory ordering guarantee.

    static int subtractIntNvAcqRel(typename AtomicTypes::Int *atomicInt,
                                   int                        value);
        // Atomically subtract from the specified 'atomicInt' the specified
        // 'value' and return the resulting value, providing the
        // acquire/release memory ordering guarantee.

    static int subtractIntNvRelaxed(typename AtomicTypes::Int *atomicInt,
                                    int                        value);
        // Atomically subtract from the specified 'atomicInt' the specified
        // 'value' and return the resulting value, without providing any memory
        // ordering guarantees.
};

                    // ====================================
                    // struct AtomicOperations_DefaultInt64
                    // ====================================

template <class IMP>
struct AtomicOperations_DefaultInt64
    // This class provides default implementations of non-essential atomic
    // operations for the 64-bit integer type independent on any specific
    // platform.  It also provides prototypes for the atomic operations for the
    // 64-bit integer type that have to be implemented separately for each
    // specific platform.  These platform-independent and platform-specific
    // atomic operations together form a full set of atomic operations for the
    // 64-bit integer type.
{
  public:
    // PUBLIC TYPES
    typedef Atomic_TypeTraits<IMP> AtomicTypes;

  private:
    // The following are signatures for the core atomics interface, the
    // implementation of which must be provided by a derived platform-specific
    // 'IMP' class.

    // NOT IMPLEMENTED
    static Types::Int64 addInt64Nv(typename AtomicTypes::Int64 *atomicInt,
                                   Types::Int64                 value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, providing the sequential consistency
        // memory ordering guarantee.

    static Types::Int64 getInt64(typename AtomicTypes::Int64 const *atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt',
        // providing the sequential consistency memory ordering guarantee.

    static void setInt64(typename AtomicTypes::Int64 *atomicInt,
                         Types::Int64                 value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', providing the sequential consistency memory
        // ordering guarantee.

    static Types::Int64 swapInt64(typename AtomicTypes::Int64 *atomicInt,
                                  Types::Int64                 swapValue);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'swapValue' and return its previous value, providing the
        // sequential consistency memory ordering guarantee.

    static Types::Int64 testAndSwapInt64(
                                     typename AtomicTypes::Int64 *atomicInt,
                                     Types::Int64                 compareValue,
                                     Types::Int64                 swapValue);
        // Conditionally set the value of the specified 'atomicInt' to the
        // specified 'swapValue' if and only if the value of 'atomicInt' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicInt', providing the sequential consistency memory
        // ordering guarantee.  The whole operation is performed atomically.

  public:
    // CLASS METHODS
    static Types::Int64 getInt64Acquire(
                                 typename AtomicTypes::Int64 const *atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt',
        // providing the acquire memory ordering guarantee.

    static Types::Int64 getInt64Relaxed(
                                 typename AtomicTypes::Int64 const *atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt', without
        // providing any memory ordering guarantees.

    static void initInt64(typename AtomicTypes::Int64 *atomicInt,
                          Types::Int64                 initialValue = 0);
        // Initialize the specified 'atomicInt' and set its value to the
        // optionally specified 'initialValue'.

    static void setInt64Relaxed(typename AtomicTypes::Int64 *atomicInt,
                                Types::Int64                 value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', without providing any memory ordering guarantees.

    static void setInt64Release(typename AtomicTypes::Int64 *atomicInt,
                                Types::Int64                 value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', providing the release memory ordering guarantee.

    static Types::Int64 swapInt64AcqRel(
                                       typename AtomicTypes::Int64 *atomicInt,
                                       Types::Int64                 swapValue);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'swapValue' and return its previous value, providing the
        // acquire/release memory ordering guarantee.

    static Types::Int64 testAndSwapInt64AcqRel(
                                     typename AtomicTypes::Int64 *atomicInt,
                                     Types::Int64                 compareValue,
                                     Types::Int64                 swapValue);
        // Conditionally set the value of the specified 'atomicInt' to the
        // specified 'swapValue' if and only if the value of 'atomicInt' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicInt', providing the sequential consistency memory
        // ordering guarantee.  The whole operation is performed atomically.

    // Arithmetic
    static void addInt64(typename AtomicTypes::Int64 *atomicInt,
                         Types::Int64                 value);
        // Atomically add to the specified 'atomicInt' the specified 'value',
        // providing the sequential consistency memory ordering guarantee.

    static void addInt64AcqRel(typename AtomicTypes::Int64 *atomicInt,
                               Types::Int64                 value);
        // Atomically add to the specified 'atomicInt' the specified 'value',
        // providing the acquire/release memory ordering guarantee.

    static void addInt64Relaxed(typename AtomicTypes::Int64 *atomicInt,
                                Types::Int64                 value);
        // Atomically add to the specified 'atomicInt' the specified 'value',
        // without providing any memory ordering guarantees.

    static Types::Int64 addInt64NvAcqRel(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, providing the acquire/release memory
        // ordering guarantee.

    static Types::Int64 addInt64NvRelaxed(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, without providing any memory
        // ordering guarantees.

    static void decrementInt64(typename AtomicTypes::Int64 *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1, providing the
        // sequential consistency memory ordering guarantee.

    static void decrementInt64AcqRel(typename AtomicTypes::Int64 *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1, providing the
        // acquire/release memory ordering guarantee.

    static Types::Int64 decrementInt64Nv(
                                       typename AtomicTypes::Int64 *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1 and return the
        // resulting value, providing the sequential consistency memory
        // ordering guarantee.

    static Types::Int64 decrementInt64NvAcqRel(
                                       typename AtomicTypes::Int64 *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1 and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    static void incrementInt64(typename AtomicTypes::Int64 *atomicInt);
        // Atomically increment the value of the specified 'atomicInt' by 1,
        // providing the sequential consistency memory ordering guarantee.

    static void incrementInt64AcqRel(typename AtomicTypes::Int64 *atomicInt);
        // Atomically increment the value of the specified 'atomicInt' by 1,
        // providing the acquire/release memory ordering guarantee.

    static Types::Int64 incrementInt64Nv(
                                       typename AtomicTypes::Int64 *atomicInt);
        // Atomically increment the specified 'atomicInt' by 1 and return the
        // resulting value, providing the sequential consistency memory
        // ordering guarantee.

    static Types::Int64 incrementInt64NvAcqRel(
                                       typename AtomicTypes::Int64 *atomicInt);
        // Atomically increment the specified 'atomicInt' by 1 and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    static Types::Int64 subtractInt64Nv(typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value);
        // Atomically subtract from the specified 'atomicInt' the specified
        // 'value' and return the resulting value, providing the sequential
        // consistency memory ordering guarantee.

    static Types::Int64 subtractInt64NvAcqRel(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value);
        // Atomically subtract from the specified 'atomicInt' the specified
        // 'value' and return the resulting value, providing the
        // acquire/release memory ordering guarantee.

    static Types::Int64 subtractInt64NvRelaxed(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value);
        // Atomically subtract from the specified 'atomicInt' the specified
        // 'value' and return the resulting value, without providing any memory
        // ordering guarantees.
};

                    // ===================================
                    // struct AtomicOperations_DefaultUint
                    // ===================================

template <class IMP>
struct Atomic_TypeTraits;

template <class IMP>
struct AtomicOperations_DefaultUint
    // This class provides default implementations of non-essential atomic
    // operations for the 32-bit unsigned integer type independent on any
    // specific platform.  It also provides prototypes for the atomic
    // operations for the 32-bit unsigned integer type that have to be
    // implemented separately for each specific platform.  These
    // platform-independent and platform-specific atomic operations together
    // form a full set of atomic operations for the 32-bit unsigned integer
    // type.
    //
    // Note that 'AtomicOperations_DefaultUint' is implemented in terms of the
    // following atomic operations on the integer type that must be provided by
    // the 'IMP' template parameter.
    //..
    //  static int getInt(typename AtomicTypes::Int const *atomicInt);
    //  static int getIntRelaxed(typename AtomicTypes::Int const *atomicInt);
    //  static int getIntAcquire(typename AtomicTypes::Int const *atomicInt);
    //  static void setInt(typename AtomicTypes::Int *atomicInt, int value);
    //  static void setIntRelaxed(typename AtomicTypes::Int *atomicInt,
    //                            int value);
    //  static void setIntRelease(typename AtomicTypes::Int *atomicInt,
    //                            int value);
    //  static int swapInt(typename AtomicTypes::Int *atomicInt,
    //                     int swapValue);
    //  static int swapIntAcqRel(typename AtomicTypes::Int *atomicInt,
    //                           int swapValue);
    //  static int testAndSwapInt(typename AtomicTypes::Int *atomicInt,
    //                            int compareValue,
    //                            int swapValue);
    //  static int testAndSwapIntAcqRel(typename AtomicTypes::Int *atomicInt,
    //                                  int compareValue,
    //                                  int swapValue);
    //..
{
  public:
    // PUBLIC TYPES
    typedef Atomic_TypeTraits<IMP> AtomicTypes;

    // CLASS METHODS
    static unsigned int getUint(typename AtomicTypes::Uint const *atomicUint);
        // Atomically retrieve the value of the specified 'atomicUint',
        // providing the sequential consistency memory ordering guarantee.

    static unsigned int getUintAcquire(
                                 typename AtomicTypes::Uint const *atomicUint);
        // Atomically retrieve the value of the specified 'atomicUint',
        // providing the acquire memory ordering guarantee.

    static unsigned int getUintRelaxed(
                                 typename AtomicTypes::Uint const *atomicUint);
        // Atomically retrieve the value of the specified 'atomicUint', without
        // providing any memory ordering guarantees.

    static void initUint(typename AtomicTypes::Uint *atomicUint,
                         unsigned int                initialValue = 0);
        // Initialize the specified 'atomicUint' and set its value to the
        // optionally specified 'initialValue'.

    static void setUint(typename AtomicTypes::Uint *atomicUint,
                        unsigned int                value);
        // Atomically set the value of the specified 'atomicUint' to the
        // specified 'value', providing the sequential consistency memory
        // ordering guarantee.

    static void setUintRelaxed(typename AtomicTypes::Uint *atomicUint,
                               unsigned int                value);
        // Atomically set the value of the specified 'atomicUint' to the
        // specified 'value', without providing any memory ordering guarantees.

    static void setUintRelease(typename AtomicTypes::Uint *atomicUint,
                               unsigned int                value);
        // Atomically set the value of the specified 'atomicUint' to the
        // specified 'value', providing the release memory ordering guarantee.

    static unsigned int swapUint(typename AtomicTypes::Uint *atomicUint,
                                 unsigned int                swapValue);
        // Atomically set the value of the specified 'atomicUint' to the
        // specified 'swapValue', and return its previous value, providing the
        // sequential consistency memory ordering guarantee.

    static unsigned int swapUintAcqRel(typename AtomicTypes::Uint *atomicUint,
                                       unsigned int                swapValue);
        // Atomically set the value of the specified 'atomicUint' to the
        // specified 'swapValue', and return its previous value, providing the
        // acquire/release memory ordering guarantee.

    static unsigned int testAndSwapUint(
                                      typename AtomicTypes::Uint *atomicUint,
                                      unsigned int                compareValue,
                                      unsigned int                swapValue);
        // Conditionally set the value of the specified 'atomicUint' to the
        // specified 'swapValue' if and only if the value of 'atomicUint'
        // equals the value of the specified 'compareValue', and return the
        // initial value of 'atomicUint', providing the sequential consistency
        // memory ordering guarantee.  The whole operation is performed
        // atomically.

    static unsigned int testAndSwapUintAcqRel(
                                      typename AtomicTypes::Uint *atomicUint,
                                      unsigned int                compareValue,
                                      unsigned int                swapValue);
        // Conditionally set the value of the specified 'atomicUint' to the
        // specified 'swapValue' if and only if the value of 'atomicInt' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicUint', providing the acquire/release memory ordering
        // guarantee.  The whole operation is performed atomically.

    // Arithmetic
    static void addUint(typename AtomicTypes::Uint *atomicUint,
                        unsigned int                value);
        // Atomically add to the specified 'atomicUint' the specified 'value',
        // providing the sequential consistency memory ordering guarantee.

    static void addUintAcqRel(typename AtomicTypes::Uint *atomicUint,
                              unsigned int                value);
        // Atomically add to the specified 'atomicUint' the specified 'value',
        // providing the acquire/release memory ordering guarantee.

    static void addUintRelaxed(typename AtomicTypes::Uint *atomicUint,
                               unsigned int                value);
        // Atomically add to the specified 'atomicUint' the specified 'value',
        // without providing any memory ordering guarantees.

    static unsigned int addUintNv(typename AtomicTypes::Uint *atomicUint,
                                  unsigned int                value);
        // Atomically add to the specified 'atomicUint' the specified 'value'
        // and return the resulting value, providing the sequential consistency
        // memory ordering guarantee.

    static unsigned int addUintNvAcqRel(typename AtomicTypes::Uint *atomicUint,
                                        unsigned int                value);
        // Atomically add to the specified 'atomicUint' the specified 'value'
        // and return the resulting value, providing the acquire/release memory
        // ordering guarantee.

    static unsigned int addUintNvRelaxed(
                                        typename AtomicTypes::Uint *atomicUint,
                                        unsigned int                value);
        // Atomically add to the specified 'atomicUint' the specified 'value'
        // and return the resulting value, without providing any memory
        // ordering guarantees.

    static void decrementUint(typename AtomicTypes::Uint *atomicUint);
        // Atomically decrement the value of the specified 'atomicUint' by 1,
        // providing the sequential consistency memory ordering guarantee.

    static void decrementUintAcqRel(typename AtomicTypes::Uint *atomicUint);
        // Atomically decrement the value of the specified 'atomicUint' by 1,
        // providing the acquire/release memory ordering guarantee.

    static unsigned int decrementUintNv(
                                       typename AtomicTypes::Uint *atomicUint);
        // Atomically decrement the specified 'atomicUint' by 1 and return the
        // resulting value, providing the sequential consistency memory
        // ordering guarantee.

    static unsigned int decrementUintNvAcqRel(
                                       typename AtomicTypes::Uint *atomicUint);
        // Atomically decrement the specified 'atomicUint' by 1 and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    static void incrementUint(typename AtomicTypes::Uint *atomicUint);
        // Atomically increment the value of the specified 'atomicUint' by 1,
        // providing the sequential consistency memory ordering guarantee.

    static void incrementUintAcqRel(typename AtomicTypes::Uint *atomicUint);
        // Atomically increment the value of the specified 'atomicUint' by 1,
        // providing the acquire/release memory ordering guarantee.

    static unsigned int incrementUintNv(
                                       typename AtomicTypes::Uint *atomicUint);
        // Atomically increment the specified 'atomicUint' by 1 and return the
        // resulting value, providing the sequential consistency memory
        // ordering guarantee.

    static unsigned int incrementUintNvAcqRel(
                                       typename AtomicTypes::Uint *atomicUint);
        // Atomically increment the specified 'atomicUint' by 1 and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    static unsigned int subtractUintNv(typename AtomicTypes::Uint *atomicUint,
                                       unsigned int                value);
        // Atomically subtract from the specified 'atomicUint' the specified
        // 'value' and return the resulting value, providing the sequential
        // consistency memory ordering guarantee.

    static unsigned int subtractUintNvAcqRel(
                                        typename AtomicTypes::Uint *atomicUint,
                                        unsigned int                value);
        // Atomically subtract from the specified 'atomicUint' the specified
        // 'value' and return the resulting value, providing the
        // acquire/release memory ordering guarantee.

    static unsigned int subtractUintNvRelaxed(
                                        typename AtomicTypes::Uint *atomicUint,
                                        unsigned int                value);
        // Atomically subtract from the specified 'atomicUint' the specified
        // 'value' and return the resulting value, without providing any memory
        // ordering guarantees.
};

                    // =====================================
                    // struct AtomicOperations_DefaultUint64
                    // =====================================

template <class IMP>
struct AtomicOperations_DefaultUint64
    // This class provides default implementations of non-essential atomic
    // operations for the 64-bit unsigned integer type independent on any
    // specific platform.  It also provides prototypes for the atomic
    // operations for the 64-bit unsigned integer type that have to be
    // implemented separately for each specific platform.  These
    // platform-independent and platform-specific atomic operations together
    // form a full set of atomic operations for the 64-bit unsigned integer
    // type.
    //
    // Note that 'AtomicOperations_DefaultUint64' is implemented in terms of
    // the following atomic operations on the Int64 type that must be provided
    // by the 'IMP' template parameter.
    //..
    //  static Types::Int64 getInt64(
    //                           typename AtomicTypes::Int64 const *atomicInt);
    //  static Types::Int64 getInt64Relaxed(
    //                           typename AtomicTypes::Int64 const *atomicInt);
    //  static Types::Int64 getInt64Acquire(
    //                           typename AtomicTypes::Int64 const *atomicInt);
    //  static void setInt64(typename AtomicTypes::Int64 *atomicInt,
    //                       Types::Int64 value);
    //  static void setInt64Relaxed(typename AtomicTypes::Int64 *atomicInt,
    //                              Types::Int64 value);
    //  static void setInt64Release(typename AtomicTypes::Int64 *atomicInt,
    //                              Types::Int64 value);
    //  static Types::Int64 swapInt64(typename AtomicTypes::Int64 *atomicInt,
    //                                Types::Int64 swapValue);
    //  static Types::Int64 swapInt64AcqRel(
    //                                  typename AtomicTypes::Int64 *atomicInt,
    //                                  Types::Int64 swapValue);
    //  static Types::Int64 testAndSwapInt64(
    //                                  typename AtomicTypes::Int64 *atomicInt,
    //                                  Types::Int64 compareValue,
    //                                  Types::Int64 swapValue);
    //  static Types::Int64 testAndSwapInt64AcqRel(
    //                                  typename AtomicTypes::Int64 *atomicInt,
    //                                  Types::Int64 compareValue,
    //                                  Types::Int64 swapValue);
    //..
{
  public:
    // PUBLIC TYPES
    typedef Atomic_TypeTraits<IMP> AtomicTypes;

    // CLASS METHODS
    static Types::Uint64 getUint64(
                               typename AtomicTypes::Uint64 const *atomicUint);
        // Atomically retrieve the value of the specified 'atomicUint',
        // providing the sequential consistency memory ordering guarantee.

    static Types::Uint64 getUint64Acquire(
                               typename AtomicTypes::Uint64 const *atomicUint);
        // Atomically retrieve the value of the specified 'atomicUint',
        // providing the acquire memory ordering guarantee.

    static Types::Uint64 getUint64Relaxed(
                               typename AtomicTypes::Uint64 const *atomicUint);
        // Atomically retrieve the value of the specified 'atomicUint', without
        // providing any memory ordering guarantees.

    static void initUint64(typename AtomicTypes::Uint64  *atomicUint,
                           Types::Uint64                  initialValue = 0);
        // Initialize the specified 'atomicUint' and set its value to the
        // optionally specified 'initialValue'.

    static void setUint64(typename AtomicTypes::Uint64 *atomicUint,
                          Types::Uint64                 value);
        // Atomically set the value of the specified 'atomicUint' to the
        // specified 'value', providing the sequential consistency memory
        // ordering guarantee.

    static void setUint64Relaxed(typename AtomicTypes::Uint64 *atomicUint,
                                 Types::Uint64                 value);
        // Atomically set the value of the specified 'atomicUint' to the
        // specified 'value', without providing any memory ordering guarantees.

    static void setUint64Release(typename AtomicTypes::Uint64 *atomicUint,
                                 Types::Uint64                 value);
        // Atomically set the value of the specified 'atomicUint' to the
        // specified 'value', providing the release memory ordering guarantee.

    static Types::Uint64 swapUint64(typename AtomicTypes::Uint64 *atomicUint,
                                    Types::Uint64                 swapValue);
        // Atomically set the value of the specified 'atomicUint' to the
        // specified 'swapValue' and return its previous value, providing the
        // sequential consistency memory ordering guarantee.

    static Types::Uint64 swapUint64AcqRel(
                                      typename AtomicTypes::Uint64 *atomicUint,
                                      Types::Uint64                 swapValue);
        // Atomically set the value of the specified 'atomicUint' to the
        // specified 'swapValue' and return its previous value, providing the
        // acquire/release memory ordering guarantee.

    static Types::Uint64 testAndSwapUint64(
                                    typename AtomicTypes::Uint64 *atomicUint,
                                    Types::Uint64                 compareValue,
                                    Types::Uint64                 swapValue);
        // Conditionally set the value of the specified 'atomicUint' to the
        // specified 'swapValue' if and only if the value of 'atomicUint'
        // equals the value of the specified 'compareValue', and return the
        // initial value of 'atomicUint', providing the sequential consistency
        // memory ordering guarantee.  The whole operation is performed
        // atomically.

    static Types::Uint64 testAndSwapUint64AcqRel(
                                    typename AtomicTypes::Uint64 *atomicUint,
                                    Types::Uint64                 compareValue,
                                    Types::Uint64                 swapValue);
        // Conditionally set the value of the specified 'atomicUint' to the
        // specified 'swapValue' if and only if the value of 'atomicUint'
        // equals the value of the specified 'compareValue', and return the
        // initial value of 'atomicUint', providing the acquire/release memory
        // ordering guarantee.  The whole operation is performed atomically.

    // Arithmetic
    static void addUint64(typename AtomicTypes::Uint64 *atomicUint,
                          Types::Uint64                 value);
        // Atomically add to the specified 'atomicUint' the specified 'value',
        // providing the sequential consistency memory ordering guarantee.

    static void addUint64AcqRel(typename AtomicTypes::Uint64 *atomicUint,
                                Types::Uint64                 value);
        // Atomically add to the specified 'atomicUint' the specified 'value',
        // providing the acquire/release memory ordering guarantee.

    static void addUint64Relaxed(typename AtomicTypes::Uint64 *atomicUint,
                                 Types::Uint64                 value);
        // Atomically add to the specified 'atomicUint' the specified 'value',
        // without providing any memory ordering guarantees.

    static Types::Uint64 addUint64Nv(typename AtomicTypes::Uint64 *atomicUint,
                                     Types::Uint64                 value);
        // Atomically add to the specified 'atomicUint' the specified 'value'
        // and return the resulting value, providing the sequential consistency
        // memory ordering guarantee.

    static Types::Uint64 addUint64NvAcqRel(
                                      typename AtomicTypes::Uint64 *atomicUint,
                                      Types::Uint64                 value);
        // Atomically add to the specified 'atomicUint' the specified 'value'
        // and return the resulting value, providing the acquire/release memory
        // ordering guarantee.

    static Types::Uint64 addUint64NvRelaxed(
                                      typename AtomicTypes::Uint64 *atomicUint,
                                      Types::Uint64                 value);
        // Atomically add to the specified 'atomicUint' the specified 'value'
        // and return the resulting value, without providing any memory
        // ordering guarantees.

    static void decrementUint64(typename AtomicTypes::Uint64 *atomicUint);
        // Atomically decrement the specified 'atomicUint' by 1, providing the
        // sequential consistency memory ordering guarantee.

    static void decrementUint64AcqRel(
                                     typename AtomicTypes::Uint64 *atomicUint);
        // Atomically decrement the specified 'atomicUint' by 1, providing the
        // acquire/release memory ordering guarantee.

    static Types::Uint64 decrementUint64Nv(
                                     typename AtomicTypes::Uint64 *atomicUint);
        // Atomically decrement the specified 'atomicUint' by 1 and return the
        // resulting value, providing the sequential consistency memory
        // ordering guarantee.

    static Types::Uint64 decrementUint64NvAcqRel(
                                     typename AtomicTypes::Uint64 *atomicUint);
        // Atomically decrement the specified 'atomicUint' by 1 and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    static void incrementUint64(typename AtomicTypes::Uint64 *atomicUint);
        // Atomically increment the value of the specified 'atomicUint' by 1,
        // providing the sequential consistency memory ordering guarantee.

    static void incrementUint64AcqRel(
                                     typename AtomicTypes::Uint64 *atomicUint);
        // Atomically increment the value of the specified 'atomicUint' by 1,
        // providing the acquire/release memory ordering guarantee.

    static Types::Uint64 incrementUint64Nv(
                                     typename AtomicTypes::Uint64 *atomicUint);
        // Atomically increment the specified 'atomicUint' by 1 and return the
        // resulting value, providing the sequential consistency memory
        // ordering guarantee.

    static Types::Uint64 incrementUint64NvAcqRel(
                                     typename AtomicTypes::Uint64 *atomicUint);
        // Atomically increment the specified 'atomicUint' by 1 and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    static Types::Uint64 subtractUint64Nv(
                                      typename AtomicTypes::Uint64 *atomicUint,
                                      Types::Uint64                 value);
        // Atomically subtract from the specified 'atomicUint' the specified
        // 'value' and return the resulting value, providing the sequential
        // consistency memory ordering guarantee.

    static Types::Uint64 subtractUint64NvAcqRel(
                                      typename AtomicTypes::Uint64 *atomicUint,
                                      Types::Uint64                 value);
        // Atomically subtract from the specified 'atomicUint' the specified
        // 'value' and return the resulting value, providing the
        // acquire/release memory ordering guarantee.

    static Types::Uint64 subtractUint64NvRelaxed(
                                      typename AtomicTypes::Uint64 *atomicUint,
                                      Types::Uint64                 value);
        // Atomically subtract from the specified 'atomicUint' the specified
        // 'value' and return the resulting value, without providing any memory
        // ordering guarantees.
};

                  // ========================================
                  // struct AtomicOperations_DefaultPointer32
                  // ========================================

template <class IMP>
struct AtomicOperations_DefaultPointer32
    // This class provides default implementations of non-essential atomic
    // operations for the 32-bit pointer type independent on any specific
    // platform.  It also provides prototypes for the atomic operations for the
    // pointer type that have to be implemented separately for each specific
    // platform.  These platform-independent and platform-specific atomic
    // operations combined together form a full set of atomic operations for
    // the pointer type.
    //
    // Note that 'AtomicOperations_DefaultPointer32' is implemented in terms of
    // the following atomic operations on the integer type that must be
    // provided by the 'IMP' template parameter.
    //..
    //  static int getInt(typename AtomicTypes::Int const *atomicInt);
    //  static int getIntRelaxed(typename AtomicTypes::Int const *atomicInt);
    //  static int getIntAcquire(typename AtomicTypes::Int const *atomicInt);
    //  static void setInt(typename AtomicTypes::Int *atomicInt, int value);
    //  static void setIntRelaxed(typename AtomicTypes::Int *atomicInt,
    //                            int value);
    //  static void setIntRelease(typename AtomicTypes::Int *atomicInt,
    //                            int value);
    //  static int swapInt(typename AtomicTypes::Int *atomicInt,
    //                     int swapValue);
    //  static int swapIntAcqRel(typename AtomicTypes::Int *atomicInt,
    //                           int swapValue);
    //  static int testAndSwapInt(typename AtomicTypes::Int *atomicInt,
    //                            int compareValue,
    //                            int swapValue);
    //  static int testAndSwapIntAcqRel(typename AtomicTypes::Int *atomicInt,
    //                                  int compareValue,
    //                                  int swapValue);
    //..
{
    // PUBLIC TYPES
    typedef Atomic_TypeTraits<IMP> AtomicTypes;

    // CLASS METHODS
    static void *getPtr(typename AtomicTypes::Pointer const *atomicPtr);
        // Atomically retrieve the value of the specified 'atomicPtr',
        // providing the sequential consistency memory ordering guarantee.

    static void *getPtrAcquire(typename AtomicTypes::Pointer const *atomicPtr);
        // Atomically retrieve the value of the specified 'atomicPtr',
        // providing the acquire memory ordering guarantee.

    static void *getPtrRelaxed(typename AtomicTypes::Pointer const *atomicPtr);
        // Atomically retrieve the value of the specified 'atomicPtr', without
        // providing any memory ordering guarantees.

    static void initPointer(typename AtomicTypes::Pointer *atomicPtr,
                            void                          *initialValue = 0);
        // Initialize the specified 'atomicPtr' and set its value to the
        // optionally specified 'initialValue'.

    static void setPtr(typename AtomicTypes::Pointer *atomicPtr,
                       void                          *value);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value', providing the sequential consistency memory
        // ordering guarantee.

    static void setPtrRelaxed(typename AtomicTypes::Pointer *atomicPtr,
                              void                          *value);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value', without providing any memory ordering guarantees.

    static void setPtrRelease(typename AtomicTypes::Pointer *atomicPtr,
                              void                          *value);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value', providing the release memory ordering guarantee.

    static void *swapPtr(typename AtomicTypes::Pointer *atomicPtr,
                         void                          *swapValue);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'swapValue', and return its previous value, providing the
        // sequential consistency memory ordering guarantee.

    static void *swapPtrAcqRel(typename AtomicTypes::Pointer *atomicPtr,
                               void                          *swapValue);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'swapValue', and return its previous value, providing the
        // acquire/release memory ordering guarantee.

    static void *testAndSwapPtr(typename AtomicTypes::Pointer *atomicPtr,
                                void                          *compareValue,
                                void                          *swapValue);
        // Conditionally set the value of the specified 'atomicPtr' to the
        // specified 'swapValue' if and only if the value of 'atomicPtr' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicPtr', providing the sequential consistency memory
        // ordering guarantee.  The whole operation is performed atomically.

    static void *testAndSwapPtrAcqRel(
                                   typename AtomicTypes::Pointer *atomicPtr,
                                   void                          *compareValue,
                                   void                          *swapValue);
        // Conditionally set the value of the specified 'atomicPtr' to the
        // specified 'swapValue' if and only if the value of 'atomicPtr' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicPtr', providing the acquire/release memory ordering
        // guarantee.  The whole operation is performed atomically.
};

                  // ========================================
                  // struct AtomicOperations_DefaultPointer64
                  // ========================================

template <class IMP>
struct AtomicOperations_DefaultPointer64
    // This class provides default implementations of non-essential atomic
    // operations for the 64-bit pointer type independent on any specific
    // platform.  It also provides prototypes for the atomic operations for the
    // pointer type that have to be implemented separately for each specific
    // platform.  These platform-independent and platform-specific atomic
    // operations combined together form a full set of atomic operations for
    // the pointer type.
    //
    // Note that 'AtomicOperations_DefaultPointer64' is implemented in terms of
    // the following atomic operations on the Int64 type that must be provided
    // by the 'IMP' template parameter.
    //..
    //  static Types::Int64 getInt64(
    //                           typename AtomicTypes::Int64 const *atomicInt);
    //  static Types::Int64 getInt64Relaxed(
    //                           typename AtomicTypes::Int64 const *atomicInt);
    //  static Types::Int64 getInt64Acquire(
    //                           typename AtomicTypes::Int64 const *atomicInt);
    //  static void setInt64(typename AtomicTypes::Int64 *atomicInt,
    //                       Types::Int64 value);
    //  static void setInt64Relaxed(typename AtomicTypes::Int64 *atomicInt,
    //                              Types::Int64 value);
    //  static void setInt64Release(typename AtomicTypes::Int64 *atomicInt,
    //                              Types::Int64 value);
    //  static Types::Int64 swapInt64(typename AtomicTypes::Int64 *atomicInt,
    //                                Types::Int64 swapValue);
    //  static Types::Int64 swapInt64AcqRel(
    //                                  typename AtomicTypes::Int64 *atomicInt,
    //                                  Types::Int64 swapValue);
    //  static Types::Int64 testAndSwapInt64(
    //                                  typename AtomicTypes::Int64 *atomicInt,
    //                                  Types::Int64 compareValue,
    //                                  Types::Int64 swapValue);
    //  static Types::Int64 testAndSwapInt64AcqRel(
    //                                  typename AtomicTypes::Int64 *atomicInt,
    //                                  Types::Int64 compareValue,
    //                                  Types::Int64 swapValue);
    //..
{
    // PUBLIC TYPES
    typedef Atomic_TypeTraits<IMP> AtomicTypes;

    // CLASS METHODS
    static void *getPtr(typename AtomicTypes::Pointer const *atomicPtr);
        // Atomically retrieve the value of the specified 'atomicPtr',
        // providing the sequential consistency memory ordering guarantee.

    static void *getPtrAcquire(typename AtomicTypes::Pointer const *atomicPtr);
        // Atomically retrieve the value of the specified 'atomicPtr',
        // providing the acquire memory ordering guarantee.

    static void *getPtrRelaxed(typename AtomicTypes::Pointer const *atomicPtr);
        // Atomically retrieve the value of the specified 'atomicPtr', without
        // providing any memory ordering guarantees.

    static void initPointer(typename AtomicTypes::Pointer *atomicPtr,
                            void                          *initialValue = 0);
        // Initialize the specified 'atomicPtr' and set its value to the
        // optionally specified 'initialValue'.

    static void setPtr(typename AtomicTypes::Pointer *atomicPtr,
                       void                          *value);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value', providing the sequential consistency memory
        // ordering guarantee.

    static void setPtrRelaxed(typename AtomicTypes::Pointer *atomicPtr,
                              void                          *value);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value', without providing any memory ordering guarantees.

    static void setPtrRelease(typename AtomicTypes::Pointer *atomicPtr,
                              void                          *value);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value', providing the release memory ordering guarantee.

    static void *swapPtr(typename AtomicTypes::Pointer *atomicPtr,
                         void                          *swapValue);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'swapValue', and return its previous value, providing the
        // sequential consistency memory ordering guarantee.

    static void *swapPtrAcqRel(typename AtomicTypes::Pointer *atomicPtr,
                               void                          *swapValue);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'swapValue', and return its previous value, providing the
        // acquire/release memory ordering guarantee.

    static void *testAndSwapPtr(typename AtomicTypes::Pointer *atomicPtr,
                                void                          *compareValue,
                                void                          *swapValue);
        // Conditionally set the value of the specified 'atomicPtr' to the
        // specified 'swapValue' if and only if the value of 'atomicPtr' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicPtr', providing the sequential consistency memory
        // ordering guarantee.  The whole operation is performed atomically.

    static void *testAndSwapPtrAcqRel(
                                   typename AtomicTypes::Pointer *atomicPtr,
                                   void                          *compareValue,
                                   void                          *swapValue);
        // Conditionally set the value of the specified 'atomicPtr' to the
        // specified 'swapValue' if and only if the value of 'atomicPtr' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicPtr', providing the acquire/release memory ordering
        // guarantee.  The whole operation is performed atomically.
};

                      // =================================
                      // struct AtomicOperations_Default32
                      // =================================

template <class IMP>
struct AtomicOperations_Default32
: AtomicOperations_DefaultInt<IMP>
, AtomicOperations_DefaultInt64<IMP>
, AtomicOperations_DefaultUint<IMP>
, AtomicOperations_DefaultUint64<IMP>
, AtomicOperations_DefaultPointer32<IMP>
    // This class provides default implementations of non-essential atomic
    // operations for the 32-bit integer, 64-bit integer, the 32-bit unsigned
    // integer, 64-bit unsigned integer and 32-bit pointer type for a generic
    // 32-bit platform.
{
};

                      // =================================
                      // struct AtomicOperations_Default64
                      // =================================

template <class IMP>
struct AtomicOperations_Default64
: AtomicOperations_DefaultInt<IMP>
, AtomicOperations_DefaultInt64<IMP>
, AtomicOperations_DefaultUint<IMP>
, AtomicOperations_DefaultUint64<IMP>
, AtomicOperations_DefaultPointer64<IMP>
    // This class provides default implementations of non-essential atomic
    // operations for the 32-bit integer, 64-bit integer, the 32-bit unsigned
    // integer, 64-bit unsigned integer and 64-bit pointer type for a generic
    // 64-bit platform.
{
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // ----------------------------------
                     // struct AtomicOperations_DefaultInt
                     // ----------------------------------

// CLASS METHODS
template <class IMP>
inline
int AtomicOperations_DefaultInt<IMP>::
    getIntAcquire(typename AtomicTypes::Int const *atomicInt)
{
    return IMP::getInt(atomicInt);
}

template <class IMP>
inline
int AtomicOperations_DefaultInt<IMP>::
    getIntRelaxed(typename AtomicTypes::Int const *atomicInt)
{
    return atomicInt->d_value;
}

template <class IMP>
inline
void AtomicOperations_DefaultInt<IMP>::
    initInt(typename AtomicTypes::Int *atomicInt, int initialValue)
{
    atomicInt->d_value = initialValue;
}

template <class IMP>
inline
void AtomicOperations_DefaultInt<IMP>::
    setIntRelaxed(typename AtomicTypes::Int *atomicInt, int value)
{
    atomicInt->d_value = value;
}

template <class IMP>
inline
void AtomicOperations_DefaultInt<IMP>::
    setIntRelease(typename AtomicTypes::Int *atomicInt, int value)
{
    IMP::setInt(atomicInt, value);
}

template <class IMP>
inline
int AtomicOperations_DefaultInt<IMP>::
    swapIntAcqRel(typename AtomicTypes::Int *atomicInt, int swapValue)
{
    return IMP::swapInt(atomicInt, swapValue);
}

template <class IMP>
inline
int AtomicOperations_DefaultInt<IMP>::testAndSwapIntAcqRel(
                                       typename AtomicTypes::Int *atomicInt,
                                       int                        compareValue,
                                       int                        swapValue)
{
    return IMP::testAndSwapInt(atomicInt, compareValue, swapValue);
}

// Arithmetic
template <class IMP>
inline
void AtomicOperations_DefaultInt<IMP>::
    addInt(typename AtomicTypes::Int *atomicInt, int value)
{
    IMP::addIntNv(atomicInt, value);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt<IMP>::
    addIntAcqRel(typename AtomicTypes::Int *atomicInt, int value)
{
    IMP::addIntNvAcqRel(atomicInt, value);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt<IMP>::
    addIntRelaxed(typename AtomicTypes::Int *atomicInt, int value)
{
    IMP::addIntNvRelaxed(atomicInt, value);
}

template <class IMP>
inline
int AtomicOperations_DefaultInt<IMP>::
    addIntNvAcqRel(typename AtomicTypes::Int *atomicInt, int value)
{
    return IMP::addIntNv(atomicInt, value);
}

template <class IMP>
inline
int AtomicOperations_DefaultInt<IMP>::
    addIntNvRelaxed(typename AtomicTypes::Int *atomicInt, int value)
{
    return IMP::addIntNvAcqRel(atomicInt, value);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt<IMP>::
    decrementInt(typename AtomicTypes::Int *atomicInt)
{
    IMP::addInt(atomicInt, -1);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt<IMP>::
    decrementIntAcqRel(typename AtomicTypes::Int *atomicInt)
{
    IMP::addIntAcqRel(atomicInt, -1);
}

template <class IMP>
inline
int AtomicOperations_DefaultInt<IMP>::
    decrementIntNv(typename AtomicTypes::Int *atomicInt)
{
    return IMP::addIntNv(atomicInt, -1);
}

template <class IMP>
inline
int AtomicOperations_DefaultInt<IMP>::
    decrementIntNvAcqRel(typename AtomicTypes::Int *atomicInt)
{
    return IMP::addIntNvAcqRel(atomicInt, -1);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt<IMP>::
    incrementInt(typename AtomicTypes::Int *atomicInt)
{
    IMP::addInt(atomicInt, 1);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt<IMP>::
    incrementIntAcqRel(typename AtomicTypes::Int *atomicInt)
{
    IMP::addIntAcqRel(atomicInt, 1);
}

template <class IMP>
inline
int AtomicOperations_DefaultInt<IMP>::
    incrementIntNv(typename AtomicTypes::Int *atomicInt)
{
    return IMP::addIntNv(atomicInt, 1);
}

template <class IMP>
inline
int AtomicOperations_DefaultInt<IMP>::
    incrementIntNvAcqRel(typename AtomicTypes::Int *atomicInt)
{
    return IMP::addIntNvAcqRel(atomicInt, 1);
}

template <class IMP>
inline
int AtomicOperations_DefaultInt<IMP>::
    subtractIntNv(typename AtomicTypes::Int *atomicInt, int value)
{
    return static_cast<int>(
        IMP::subtractUintNv(
            reinterpret_cast<typename AtomicTypes::Uint *>(atomicInt),
            static_cast<unsigned int>(value)));
}

template <class IMP>
inline
int AtomicOperations_DefaultInt<IMP>::
    subtractIntNvAcqRel(typename AtomicTypes::Int *atomicInt, int value)
{
    return static_cast<int>(
        IMP::subtractUintNvAcqRel(
            reinterpret_cast<typename AtomicTypes::Uint *>(atomicInt),
            static_cast<unsigned int>(value)));
}

template <class IMP>
inline
int AtomicOperations_DefaultInt<IMP>::
    subtractIntNvRelaxed(typename AtomicTypes::Int *atomicInt, int value)
{
    return static_cast<int>(
        IMP::subtractUintNvRelaxed(
            reinterpret_cast<typename AtomicTypes::Uint *>(atomicInt),
            static_cast<unsigned int>(value)));
}

                    // ------------------------------------
                    // struct AtomicOperations_DefaultInt64
                    // ------------------------------------

// CLASS METHODS
template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::
    getInt64Acquire(typename AtomicTypes::Int64 const *atomicInt)
{
    return IMP::getInt64(atomicInt);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::
    getInt64Relaxed(typename AtomicTypes::Int64 const *atomicInt)
{
    return atomicInt->d_value;
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::initInt64(
                                    typename AtomicTypes::Int64  *atomicInt,
                                    Types::Int64                  initialValue)
{
    atomicInt->d_value = initialValue;
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::setInt64Relaxed(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value)
{
    atomicInt->d_value = value;
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::setInt64Release(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value)
{
    IMP::setInt64(atomicInt, value);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::swapInt64AcqRel(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 swapValue)
{
    return IMP::swapInt64(atomicInt, swapValue);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::testAndSwapInt64AcqRel(
                                     typename AtomicTypes::Int64 *atomicInt,
                                     Types::Int64                 compareValue,
                                     Types::Int64                 swapValue)
{
    return IMP::testAndSwapInt64(atomicInt, compareValue, swapValue);
}

// Arithmetic
template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::addInt64(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value)
{
    IMP::addInt64Nv(atomicInt, value);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::addInt64AcqRel(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value)
{
    IMP::addInt64NvAcqRel(atomicInt, value);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::addInt64Relaxed(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value)
{
    IMP::addInt64NvRelaxed(atomicInt, value);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::addInt64NvAcqRel(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value)
{
    return IMP::addInt64Nv(atomicInt, value);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::addInt64NvRelaxed(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value)
{
    return IMP::addInt64NvAcqRel(atomicInt, value);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::
    decrementInt64(typename AtomicTypes::Int64 *atomicInt)
{
    IMP::addInt64(atomicInt, -1);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::
    decrementInt64AcqRel(typename AtomicTypes::Int64 *atomicInt)
{
    IMP::addInt64AcqRel(atomicInt, -1);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::
    decrementInt64Nv(typename AtomicTypes::Int64 *atomicInt)
{
    return IMP::addInt64Nv(atomicInt, -1);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::
    decrementInt64NvAcqRel(typename AtomicTypes::Int64 *atomicInt)
{
    return IMP::addInt64NvAcqRel(atomicInt, -1);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::
    incrementInt64(typename AtomicTypes::Int64 *atomicInt)
{
    IMP::addInt64(atomicInt, 1);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::
    incrementInt64AcqRel(typename AtomicTypes::Int64 *atomicInt)
{
    IMP::addInt64AcqRel(atomicInt, 1);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::
    incrementInt64Nv(typename AtomicTypes::Int64 *atomicInt)
{
    return IMP::addInt64Nv(atomicInt, 1);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::
    incrementInt64NvAcqRel(typename AtomicTypes::Int64 *atomicInt)
{
    return IMP::addInt64NvAcqRel(atomicInt, 1);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::subtractInt64Nv(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value)
{
    return static_cast<Types::Int64>(
        IMP::subtractUint64Nv(
            reinterpret_cast<typename AtomicTypes::Uint64 *>(atomicInt),
            static_cast<Types::Uint64>(value)));
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::subtractInt64NvAcqRel(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value)
{
    return static_cast<Types::Int64>(
        IMP::subtractUint64NvAcqRel(
            reinterpret_cast<typename AtomicTypes::Uint64 *>(atomicInt),
            static_cast<Types::Uint64>(value)));
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::subtractInt64NvRelaxed(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64                 value)
{
    return static_cast<Types::Int64>(
        IMP::subtractUint64NvRelaxed(
            reinterpret_cast<typename AtomicTypes::Uint64 *>(atomicInt),
            static_cast<Types::Uint64>(value)));
}

                     // ----------------------------------
                     // struct AtomicOperations_DefaultUint
                     // ----------------------------------

// CLASS METHODS
template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::
    getUint(typename AtomicTypes::Uint const *atomicUint)
{
    return static_cast<unsigned int>(
              IMP::getInt(
                  reinterpret_cast<typename AtomicTypes::Int const *>(
                      atomicUint)));
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::
    getUintAcquire(typename AtomicTypes::Uint const *atomicUint)
{
    return static_cast<unsigned int>(
        IMP::getIntAcquire(
             reinterpret_cast<typename AtomicTypes::Int const *>(atomicUint)));
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::
    getUintRelaxed(typename AtomicTypes::Uint const *atomicUint)
{
    return atomicUint->d_value;
}

template <class IMP>
inline
void AtomicOperations_DefaultUint<IMP>::
    initUint(typename AtomicTypes::Uint *atomicUint, unsigned int initialValue)
{
    atomicUint->d_value = initialValue;
}

template <class IMP>
inline
void AtomicOperations_DefaultUint<IMP>::
    setUint(typename AtomicTypes::Uint *atomicUint, unsigned int value)
{
    IMP::setInt(reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
                static_cast<int>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultUint<IMP>::
    setUintRelaxed(typename AtomicTypes::Uint *atomicUint, unsigned int value)
{
    atomicUint->d_value = value;
}

template <class IMP>
inline
void AtomicOperations_DefaultUint<IMP>::
    setUintRelease(typename AtomicTypes::Uint *atomicUint, unsigned int value)
{
    IMP::setIntRelease(
        reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
                static_cast<int>(value));
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::
    swapUint(typename AtomicTypes::Uint *atomicUint, unsigned int swapValue)
{
    return static_cast<unsigned int>(
             IMP::swapInt(
                 reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
                 static_cast<int>(swapValue)));
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::swapUintAcqRel(
                                        typename AtomicTypes::Uint *atomicUint,
                                        unsigned int                swapValue)
{
    return static_cast<unsigned int>(
        IMP::swapIntAcqRel(
                reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
                static_cast<int>(swapValue)));
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::testAndSwapUint(
                           typename AtomicTypes::Uint            *atomicUint,
                           unsigned int                           compareValue,
                           unsigned int                           swapValue)
{
    return static_cast<unsigned int>(
             IMP::testAndSwapInt(
                 reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
                                 static_cast<int>(compareValue),
                                 static_cast<int>(swapValue)));
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::testAndSwapUintAcqRel(
                                      typename AtomicTypes::Uint *atomicUint,
                                      unsigned int                compareValue,
                                      unsigned int                swapValue)
{
    return static_cast<unsigned int>(
             IMP::testAndSwapIntAcqRel(
                 reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
                                 static_cast<int>(compareValue),
                                 static_cast<int>(swapValue)));
}

// Arithmetic
template <class IMP>
inline
void AtomicOperations_DefaultUint<IMP>::
    addUint(typename AtomicTypes::Uint *atomicUint, unsigned int value)
{
        IMP::addInt(reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
                        static_cast<int>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultUint<IMP>::
    addUintAcqRel(typename AtomicTypes::Uint *atomicUint, unsigned int value)
{
    IMP::addIntAcqRel(
        reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
        static_cast<int>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultUint<IMP>::
    addUintRelaxed(typename AtomicTypes::Uint *atomicUint, unsigned int value)
{
    IMP::addIntRelaxed(
        reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
        static_cast<int>(value));
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::
    addUintNv(typename AtomicTypes::Uint *atomicUint, unsigned int value)
{
    return static_cast<unsigned int>(
             IMP::addIntNv(
                 reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
                     static_cast<int>(value)));
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::
    addUintNvAcqRel(typename AtomicTypes::Uint *atomicUint, unsigned int value)
{
    return static_cast<unsigned int>(
        IMP::addIntNvAcqRel(
                reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
                static_cast<int>(value)));
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::addUintNvRelaxed(
                                        typename AtomicTypes::Uint *atomicUint,
                                        unsigned int                value)
{
    return static_cast<unsigned int>(
        IMP::addIntNvRelaxed(
                reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
                static_cast<int>(value)));
}

template <class IMP>
inline
void AtomicOperations_DefaultUint<IMP>::
    decrementUint(typename AtomicTypes::Uint *atomicUint)
{
    IMP::addInt(reinterpret_cast<typename AtomicTypes::Int *>(atomicUint), -1);
}

template <class IMP>
inline
void AtomicOperations_DefaultUint<IMP>::
    decrementUintAcqRel(typename AtomicTypes::Uint *atomicUint)
{
    IMP::addIntAcqRel(
                reinterpret_cast<typename AtomicTypes::Int *>(atomicUint), -1);
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::
    decrementUintNv(typename AtomicTypes::Uint *atomicUint)
{
    return IMP::subtractUintNv(atomicUint, 1);
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::
    decrementUintNvAcqRel(typename AtomicTypes::Uint *atomicUint)
{
    return static_cast<unsigned int>(
       IMP::addIntNvAcqRel(
               reinterpret_cast<typename AtomicTypes::Int *>(atomicUint), -1));
}

template <class IMP>
inline
void AtomicOperations_DefaultUint<IMP>::
    incrementUint(typename AtomicTypes::Uint *atomicUint)
{
    IMP::addUint(atomicUint, 1);
}

template <class IMP>
inline
void AtomicOperations_DefaultUint<IMP>::
    incrementUintAcqRel(typename AtomicTypes::Uint *atomicUint)
{
    IMP::addUintAcqRel(atomicUint, 1);
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::
    incrementUintNv(typename AtomicTypes::Uint *atomicUint)
{
    return IMP::addUintNv(atomicUint, 1);
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::
    incrementUintNvAcqRel(typename AtomicTypes::Uint *atomicUint)
{
    return IMP::addUintNvAcqRel(atomicUint, 1);
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::
    subtractUintNv(typename AtomicTypes::Uint *atomicUint, unsigned int value)
{
    return static_cast<unsigned int>(
             IMP::addIntNv(
                 reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
                     static_cast<int>(-value)));
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::subtractUintNvAcqRel(
                                        typename AtomicTypes::Uint *atomicUint,
                                        unsigned int                value)
{
    return static_cast<unsigned int>(
        IMP::addIntNvAcqRel(
                reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
                static_cast<int>(-value)));
}

template <class IMP>
inline
unsigned int AtomicOperations_DefaultUint<IMP>::subtractUintNvRelaxed(
                                        typename AtomicTypes::Uint *atomicUint,
                                        unsigned int                value)
{
    return static_cast<unsigned int>(
        IMP::addIntNvRelaxed(
                reinterpret_cast<typename AtomicTypes::Int *>(atomicUint),
                static_cast<int>(-value)));
}

                    // ------------------------------------
                    // struct AtomicOperations_DefaultUint64
                    // ------------------------------------

// CLASS METHODS
template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::
    getUint64(typename AtomicTypes::Uint64 const *atomicUint)
{
    return static_cast<Types::Uint64>(
        IMP::getInt64(reinterpret_cast<typename AtomicTypes::Int64 const *>(
                      atomicUint)));
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::
    getUint64Acquire(typename AtomicTypes::Uint64 const *atomicUint)
{
    return static_cast<Types::Uint64>(IMP::getInt64Acquire(
        reinterpret_cast<typename AtomicTypes::Int64 const *>(atomicUint)));
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::
    getUint64Relaxed(typename AtomicTypes::Uint64 const *atomicUint)
{
    return atomicUint->d_value;
}

template <class IMP>
inline
void AtomicOperations_DefaultUint64<IMP>::initUint64(
                                   typename AtomicTypes::Uint64  *atomicUint,
                                   Types::Uint64                  initialValue)
{
    atomicUint->d_value = initialValue;
}

template <class IMP>
inline
void AtomicOperations_DefaultUint64<IMP>::
    setUint64(typename AtomicTypes::Uint64 *atomicUint, Types::Uint64 value)
{
    IMP::setInt64(reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
                  static_cast<Types::Int64>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultUint64<IMP>::setUint64Relaxed(
                                      typename AtomicTypes::Uint64 *atomicUint,
                                      Types::Uint64                 value)
{
    atomicUint->d_value = value;
}

template <class IMP>
inline
void AtomicOperations_DefaultUint64<IMP>::setUint64Release(
                                      typename AtomicTypes::Uint64 *atomicUint,
                                      Types::Uint64                 value)
{
    IMP::setInt64Release(
        reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
        static_cast<Types::Int64>(value));
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::swapUint64(
                                      typename AtomicTypes::Uint64 *atomicUint,
                                      Types::Uint64                 swapValue)
{
    return static_cast<Types::Uint64>(
              IMP::swapInt64(
                   reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
                   static_cast<Types::Uint64>(swapValue)));
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::swapUint64AcqRel(
                                      typename AtomicTypes::Uint64 *atomicUint,
                                      Types::Uint64                 swapValue)
{
    return static_cast<Types::Uint64>(
        IMP::swapInt64AcqRel(
            reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
            static_cast<Types::Int64>(swapValue)));
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::testAndSwapUint64(
                                    typename AtomicTypes::Uint64 *atomicUint,
                                    Types::Uint64                 compareValue,
                                    Types::Uint64                 swapValue)
{
    return static_cast<Types::Uint64>(
              IMP::testAndSwapInt64(
                  reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
                  static_cast<Types::Int64>(compareValue),
                  static_cast<Types::Int64>(swapValue)));
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::testAndSwapUint64AcqRel(
                                    typename AtomicTypes::Uint64 *atomicUint,
                                    Types::Uint64                 compareValue,
                                    Types::Uint64                 swapValue)
{
    return static_cast<Types::Uint64>(
        IMP::testAndSwapInt64AcqRel(
                  reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
                  static_cast<Types::Int64>(compareValue),
                  static_cast<Types::Int64>(swapValue)));
}

// Arithmetic
template <class IMP>
inline
void AtomicOperations_DefaultUint64<IMP>::addUint64(
                  typename AtomicTypes::Uint64 *atomicUint,Types::Uint64 value)
{
    IMP::addInt64(reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
                     static_cast<Types::Int64>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultUint64<IMP>::addUint64AcqRel(
                  typename AtomicTypes::Uint64 *atomicUint,Types::Uint64 value)
{
    IMP::addInt64AcqRel(
        reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
        static_cast<Types::Int64>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultUint64<IMP>::addUint64Relaxed(
                  typename AtomicTypes::Uint64 *atomicUint,Types::Uint64 value)
{
    IMP::addInt64Relaxed(
        reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
        static_cast<Types::Int64>(value));
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::addUint64Nv(
                  typename AtomicTypes::Uint64 *atomicUint,Types::Uint64 value)
{
    return static_cast<Types::Uint64>(
              IMP::addInt64Nv(
                  reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
                  static_cast<Types::Int64>(value)));
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::addUint64NvAcqRel(
                  typename AtomicTypes::Uint64 *atomicUint,Types::Uint64 value)
{
    return static_cast<Types::Uint64>(
        IMP::addInt64NvAcqRel(
            reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
            static_cast<Types::Int64>(value)));
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::addUint64NvRelaxed(
                  typename AtomicTypes::Uint64 *atomicUint,Types::Uint64 value)
{
    return static_cast<Types::Uint64>(
        IMP::addInt64NvRelaxed(
            reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
            static_cast<Types::Int64>(value)));
}

template <class IMP>
inline
void AtomicOperations_DefaultUint64<IMP>::
    decrementUint64(typename AtomicTypes::Uint64 *atomicUint)
{
    IMP::addInt64(reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
                  -1);
}

template <class IMP>
inline
void AtomicOperations_DefaultUint64<IMP>::
    decrementUint64AcqRel(typename AtomicTypes::Uint64 *atomicUint)
{
        IMP::addInt64AcqRel(
              reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint), -1);
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::
    decrementUint64Nv(typename AtomicTypes::Uint64 *atomicUint)
{
    return IMP::subtractUint64Nv(atomicUint, 1);
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::
    decrementUint64NvAcqRel(typename AtomicTypes::Uint64 *atomicUint)
{
    return static_cast<Types::Uint64>(
        IMP::addInt64NvAcqRel(
             reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint), -1));
}

template <class IMP>
inline
void AtomicOperations_DefaultUint64<IMP>::
    incrementUint64(typename AtomicTypes::Uint64 *atomicUint)
{
    IMP::addUint64(atomicUint, 1);
}

template <class IMP>
inline
void AtomicOperations_DefaultUint64<IMP>::
    incrementUint64AcqRel(typename AtomicTypes::Uint64 *atomicUint)
{
    IMP::addUint64AcqRel(atomicUint, 1);
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::
    incrementUint64Nv(typename AtomicTypes::Uint64 *atomicUint)
{
    return IMP::addUint64Nv(atomicUint, 1);
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::
    incrementUint64NvAcqRel(typename AtomicTypes::Uint64 *atomicUint)
{
    return IMP::addUint64NvAcqRel(atomicUint, 1);
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::subtractUint64Nv(
                 typename AtomicTypes::Uint64 *atomicUint, Types::Uint64 value)
{
    return static_cast<Types::Uint64>(
              IMP::addInt64Nv(
                  reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
                  static_cast<Types::Int64>(-value)));
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::subtractUint64NvAcqRel(
                  typename AtomicTypes::Uint64 *atomicUint,Types::Uint64 value)
{
    return static_cast<Types::Uint64>(
        IMP::addInt64NvAcqRel(
            reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
            static_cast<Types::Int64>(-value)));
}

template <class IMP>
inline
Types::Uint64 AtomicOperations_DefaultUint64<IMP>::subtractUint64NvRelaxed(
                  typename AtomicTypes::Uint64 *atomicUint,Types::Uint64 value)
{
    return static_cast<Types::Uint64>(
        IMP::addInt64NvRelaxed(
            reinterpret_cast<typename AtomicTypes::Int64 *>(atomicUint),
            static_cast<Types::Int64>(-value)));
}

                  // ----------------------------------------
                  // struct AtomicOperations_DefaultPointer32
                  // ----------------------------------------

// CLASS METHODS
template <class IMP>
inline
void *AtomicOperations_DefaultPointer32<IMP>::
    getPtr(typename AtomicTypes::Pointer const *atomicPtr)
{
    return reinterpret_cast<void *>(
             IMP::getInt(
                reinterpret_cast<typename AtomicTypes::Int const *>(
                    atomicPtr)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer32<IMP>::
    getPtrAcquire(typename AtomicTypes::Pointer const *atomicPtr)
{
    return reinterpret_cast<void *>(
             IMP::getIntAcquire(
                reinterpret_cast<typename AtomicTypes::Int const *>(
                    atomicPtr)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer32<IMP>::
    getPtrRelaxed(typename AtomicTypes::Pointer const *atomicPtr)
{
    return reinterpret_cast<void *>(
             IMP::getIntRelaxed(
                reinterpret_cast<typename AtomicTypes::Int const *>(
                    atomicPtr)));
}

template <class IMP>
inline
void AtomicOperations_DefaultPointer32<IMP>::initPointer(
                  typename AtomicTypes::Pointer *atomicPtr, void *initialValue)
{
    atomicPtr->d_value = initialValue;
}

template <class IMP>
inline
void AtomicOperations_DefaultPointer32<IMP>::setPtr(
                         typename AtomicTypes::Pointer *atomicPtr, void *value)
{
    IMP::setInt(
        reinterpret_cast<typename AtomicTypes::Int *>(atomicPtr),
        reinterpret_cast<Types::IntPtr>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultPointer32<IMP>::setPtrRelaxed(
                         typename AtomicTypes::Pointer *atomicPtr, void *value)
{
    IMP::setIntRelaxed(
        reinterpret_cast<typename AtomicTypes::Int *>(atomicPtr),
        reinterpret_cast<Types::IntPtr>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultPointer32<IMP>::setPtrRelease(
                         typename AtomicTypes::Pointer *atomicPtr, void *value)
{
    IMP::setIntRelease(
        reinterpret_cast<typename AtomicTypes::Int *>(atomicPtr),
        reinterpret_cast<Types::IntPtr>(value));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer32<IMP>::swapPtr(
                     typename AtomicTypes::Pointer *atomicPtr, void *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::swapInt(
                reinterpret_cast<typename AtomicTypes::Int *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer32<IMP>::swapPtrAcqRel(
                     typename AtomicTypes::Pointer *atomicPtr, void *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::swapIntAcqRel(
                reinterpret_cast<typename AtomicTypes::Int *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer32<IMP>::testAndSwapPtr(
                                   typename AtomicTypes::Pointer *atomicPtr,
                                   void                          *compareValue,
                                   void                          *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::testAndSwapInt(
                reinterpret_cast<typename AtomicTypes::Int *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(compareValue),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer32<IMP>::testAndSwapPtrAcqRel(
                                   typename AtomicTypes::Pointer *atomicPtr,
                                   void                          *compareValue,
                                   void                          *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::testAndSwapIntAcqRel(
                reinterpret_cast<typename AtomicTypes::Int *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(compareValue),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

                  // ----------------------------------------
                  // struct AtomicOperations_DefaultPointer64
                  // ----------------------------------------

// CLASS METHODS
template <class IMP>
inline
void *AtomicOperations_DefaultPointer64<IMP>::
    getPtr(typename AtomicTypes::Pointer const *atomicPtr)
{
    return reinterpret_cast<void *>(
             IMP::getInt64(
                reinterpret_cast<typename AtomicTypes::Int64 const *>(
                    atomicPtr)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer64<IMP>::
    getPtrAcquire(typename AtomicTypes::Pointer const *atomicPtr)
{
    return reinterpret_cast<void *>(
             IMP::getInt64Acquire(
                reinterpret_cast<typename AtomicTypes::Int64 const *>(
                    atomicPtr)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer64<IMP>::
    getPtrRelaxed(typename AtomicTypes::Pointer const *atomicPtr)
{
    return reinterpret_cast<void *>(
             IMP::getInt64Relaxed(
                reinterpret_cast<typename AtomicTypes::Int64 const *>(
                    atomicPtr)));
}

template <class IMP>
inline
void AtomicOperations_DefaultPointer64<IMP>::initPointer(
                  typename AtomicTypes::Pointer *atomicPtr, void *initialValue)
{
    atomicPtr->d_value = initialValue;
}

template <class IMP>
inline
void AtomicOperations_DefaultPointer64<IMP>::setPtr(
                         typename AtomicTypes::Pointer *atomicPtr, void *value)
{
    IMP::setInt64(
        reinterpret_cast<typename AtomicTypes::Int64 *>(atomicPtr),
        reinterpret_cast<Types::IntPtr>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultPointer64<IMP>::setPtrRelaxed(
                         typename AtomicTypes::Pointer *atomicPtr, void *value)
{
    IMP::setInt64Relaxed(
        reinterpret_cast<typename AtomicTypes::Int64 *>(atomicPtr),
        reinterpret_cast<Types::IntPtr>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultPointer64<IMP>::setPtrRelease(
                         typename AtomicTypes::Pointer *atomicPtr, void *value)
{
    IMP::setInt64Release(
        reinterpret_cast<typename AtomicTypes::Int64 *>(atomicPtr),
        reinterpret_cast<Types::IntPtr>(value));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer64<IMP>::swapPtr(
                     typename AtomicTypes::Pointer *atomicPtr, void *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::swapInt64(
                reinterpret_cast<typename AtomicTypes::Int64 *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer64<IMP>::swapPtrAcqRel(
                     typename AtomicTypes::Pointer *atomicPtr, void *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::swapInt64AcqRel(
                reinterpret_cast<typename AtomicTypes::Int64 *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer64<IMP>::testAndSwapPtr(
                                   typename AtomicTypes::Pointer *atomicPtr,
                                   void                          *compareValue,
                                   void                          *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::testAndSwapInt64(
                reinterpret_cast<typename AtomicTypes::Int64 *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(compareValue),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer64<IMP>::testAndSwapPtrAcqRel(
                                   typename AtomicTypes::Pointer *atomicPtr,
                                   void                          *compareValue,
                                   void                          *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::testAndSwapInt64AcqRel(
                reinterpret_cast<typename AtomicTypes::Int64 *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(compareValue),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

}  // close package namespace

}  // close enterprise namespace

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
