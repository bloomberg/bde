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
// default implementation may be overriden for a target platform).
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
//:                                      AtomicOperations_DefaultInt64,
//:                                      AtomicOperations_DefaultPointer32
//: o bsls::AtomicOperations_Default64 : AtomicOperations_DefaultInt,
//:                                      AtomicOperations_DefaultInt64,
//:                                      AtomicOperations_DefaultPointer64
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
// were overriden.
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
    static int getInt(typename AtomicTypes::Int const *atomicInt);

    static void setInt(typename AtomicTypes::Int *atomicInt, int value);

    static int swapInt(typename AtomicTypes::Int *atomicInt, int swapValue);

    static int testAndSwapInt(typename AtomicTypes::Int *atomicInt,
                              int compareValue,
                              int swapValue);

    static int addIntNv(typename AtomicTypes::Int *atomicInt, int value);

  public:
    // CLASS METHODS
    static void initInt(typename AtomicTypes::Int *atomicInt,
                        int initialValue = 0);

    static int getIntRelaxed(typename AtomicTypes::Int const *atomicInt);

    static int getIntAcquire(typename AtomicTypes::Int const *atomicInt);

    static void setIntRelaxed(typename AtomicTypes::Int *atomicInt, int value);

    static void setIntRelease(typename AtomicTypes::Int *atomicInt, int value);

    static int swapIntAcqRel(typename AtomicTypes::Int *atomicInt,
                             int swapValue);

    static int testAndSwapIntAcqRel(typename AtomicTypes::Int *atomicInt,
                                    int compareValue,
                                    int swapValue);

    static int addIntNvRelaxed(typename AtomicTypes::Int *atomicInt,
                               int value);

    static int addIntNvAcqRel(typename AtomicTypes::Int *atomicInt, int value);

    static void addInt(typename AtomicTypes::Int *atomicInt, int value);

    static void addIntRelaxed(typename AtomicTypes::Int *atomicInt, int value);

    static void addIntAcqRel(typename AtomicTypes::Int *atomicInt, int value);

    static int incrementIntNv(typename AtomicTypes::Int *atomicInt);

    static int incrementIntNvAcqRel(typename AtomicTypes::Int *atomicInt);

    static void incrementInt(typename AtomicTypes::Int *atomicInt);

    static void incrementIntAcqRel(typename AtomicTypes::Int *atomicInt);

    static int decrementIntNv(typename AtomicTypes::Int *atomicInt);

    static int decrementIntNvAcqRel(typename AtomicTypes::Int *atomicInt);

    static void decrementInt(typename AtomicTypes::Int *atomicInt);

    static void decrementIntAcqRel(typename AtomicTypes::Int *atomicInt);
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
    static Types::Int64 getInt64(typename AtomicTypes::Int64 const *atomicInt);

    static void setInt64(typename AtomicTypes::Int64 *atomicInt,
                         Types::Int64 value);

    static Types::Int64 swapInt64(typename AtomicTypes::Int64 *atomicInt,
                                  Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64 compareValue,
                                        Types::Int64 swapValue);

    static Types::Int64 addInt64Nv(typename AtomicTypes::Int64 *atomicInt,
                                   Types::Int64 value);

  public:
    // CLASS METHODS
    static void initInt64(typename AtomicTypes::Int64  *atomicInt,
                          Types::Int64  initialValue = 0);

    static Types::Int64 getInt64Relaxed(
                                 typename AtomicTypes::Int64 const *atomicInt);

    static Types::Int64 getInt64Acquire(
                                 typename AtomicTypes::Int64 const *atomicInt);

    static void setInt64Relaxed(typename AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static void setInt64Release(typename AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static Types::Int64 swapInt64AcqRel(typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64AcqRel(
                                      typename AtomicTypes::Int64 *atomicInt,
                                      Types::Int64 compareValue,
                                      Types::Int64 swapValue);

    static Types::Int64 addInt64NvRelaxed(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64 value);

    static Types::Int64 addInt64NvAcqRel(
                                        typename AtomicTypes::Int64 *atomicInt,
                                        Types::Int64 value);

    static void addInt64(typename AtomicTypes::Int64 *atomicInt,
                         Types::Int64 value);

    static void addInt64Relaxed(typename AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static void addInt64AcqRel(typename AtomicTypes::Int64 *atomicInt,
                               Types::Int64 value);

    static void incrementInt64(typename AtomicTypes::Int64 *atomicInt);

    static void incrementInt64AcqRel(typename AtomicTypes::Int64 *atomicInt);

    static Types::Int64 incrementInt64Nv(
                                       typename AtomicTypes::Int64 *atomicInt);

    static Types::Int64 incrementInt64NvAcqRel(
                                       typename AtomicTypes::Int64 *atomicInt);

    static void decrementInt64(typename AtomicTypes::Int64 *atomicInt);

    static void decrementInt64AcqRel(typename AtomicTypes::Int64 *atomicInt);

    static Types::Int64 decrementInt64Nv(
                                       typename AtomicTypes::Int64 *atomicInt);

    static Types::Int64 decrementInt64NvAcqRel(
                                       typename AtomicTypes::Int64 *atomicInt);
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
    // Note that 'AtomicOperations_DefaultPointer32' is implemented in
    // terms of the following atomic operations on the integer type which must
    // be provided by the 'IMP' template parameter.
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
    static void initPointer(typename AtomicTypes::Pointer *atomicPtr,
                            void *initialValue = 0);

    static void *getPtr(typename AtomicTypes::Pointer const *atomicPtr);

    static void *getPtrRelaxed(typename AtomicTypes::Pointer const *atomicPtr);

    static void *getPtrAcquire(typename AtomicTypes::Pointer const *atomicPtr);

    static void setPtr(typename AtomicTypes::Pointer *atomicPtr,
                       void *value);

    static void setPtrRelaxed(typename AtomicTypes::Pointer *atomicPtr,
                              void *value);

    static void setPtrRelease(typename AtomicTypes::Pointer *atomicPtr,
                              void *value);

    static void *swapPtr(typename AtomicTypes::Pointer *atomicPtr,
                         void *swapValue);

    static void *swapPtrAcqRel(typename AtomicTypes::Pointer *atomicPtr,
                               void *swapValue);

    static void *testAndSwapPtr(typename AtomicTypes::Pointer *atomicPtr,
                                void *compareValue,
                                void *swapValue);

    static void *testAndSwapPtrAcqRel(typename AtomicTypes::Pointer *atomicPtr,
                                      void *compareValue,
                                      void *swapValue);
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
    // Note that 'AtomicOperations_DefaultPointer64' is implemented in
    // terms of the following atomic operations on the Int64 type which must be
    // provided by the 'IMP' template parameter.
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
    static void initPointer(typename AtomicTypes::Pointer *atomicPtr,
                            void *initialValue = 0);

    static void *getPtr(typename AtomicTypes::Pointer const *atomicPtr);

    static void *getPtrRelaxed(typename AtomicTypes::Pointer const *atomicPtr);

    static void *getPtrAcquire(typename AtomicTypes::Pointer const *atomicPtr);

    static void setPtr(typename AtomicTypes::Pointer *atomicPtr,
                       void *value);

    static void setPtrRelaxed(typename AtomicTypes::Pointer *atomicPtr,
                              void *value);

    static void setPtrRelease(typename AtomicTypes::Pointer *atomicPtr,
                              void *value);

    static void *swapPtr(typename AtomicTypes::Pointer *atomicPtr,
                         void *swapValue);

    static void *swapPtrAcqRel(typename AtomicTypes::Pointer *atomicPtr,
                               void *swapValue);

    static void *testAndSwapPtr(typename AtomicTypes::Pointer   *atomicPtr,
                                void *compareValue,
                                void *swapValue);

    static void *testAndSwapPtrAcqRel(typename AtomicTypes::Pointer *atomicPtr,
                                      void *compareValue,
                                      void *swapValue);
};

                      // =================================
                      // struct AtomicOperations_Default32
                      // =================================

template <class IMP>
struct AtomicOperations_Default32
: AtomicOperations_DefaultInt<IMP>
, AtomicOperations_DefaultInt64<IMP>
, AtomicOperations_DefaultPointer32<IMP>
    // This class provides default implementations of non-essential atomic
    // operations for the 32-bit integer, 64-bit integer and 32-bit pointer
    // type for a generic 32-bit platform.
{
};

                      // =================================
                      // struct AtomicOperations_Default64
                      // =================================

template <class IMP>
struct AtomicOperations_Default64
: AtomicOperations_DefaultInt<IMP>
, AtomicOperations_DefaultInt64<IMP>
, AtomicOperations_DefaultPointer64<IMP>
    // This class provides default implementations of non-essential atomic
    // operations for the 32-bit integer, 64-bit integer and 64-bit pointer
    // type for a generic 64-bit platform.
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
void AtomicOperations_DefaultInt<IMP>::
    initInt(typename AtomicTypes::Int *atomicInt, int initialValue)
{
    atomicInt->d_value = initialValue;
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
int AtomicOperations_DefaultInt<IMP>::
    getIntAcquire(typename AtomicTypes::Int const *atomicInt)
{
    return IMP::getInt(atomicInt);
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
int AtomicOperations_DefaultInt<IMP>::
    testAndSwapIntAcqRel(typename AtomicTypes::Int *atomicInt,
                         int compareValue,
                         int swapValue)
{
    return IMP::testAndSwapInt(atomicInt, compareValue, swapValue);
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
int AtomicOperations_DefaultInt<IMP>::
    addIntNvAcqRel(typename AtomicTypes::Int *atomicInt, int value)
{
    return IMP::addIntNv(atomicInt, value);
}

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
    addIntRelaxed(typename AtomicTypes::Int *atomicInt, int value)
{
    IMP::addIntNvRelaxed(atomicInt, value);
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

                    // ------------------------------------
                    // struct AtomicOperations_DefaultInt64
                    // ------------------------------------

// CLASS METHODS
template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::
    initInt64(typename AtomicTypes::Int64  *atomicInt,
              Types::Int64 initialValue)
{
    atomicInt->d_value = initialValue;
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
Types::Int64 AtomicOperations_DefaultInt64<IMP>::
    getInt64Acquire(typename AtomicTypes::Int64 const *atomicInt)
{
    return IMP::getInt64(atomicInt);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::
    setInt64Relaxed(typename AtomicTypes::Int64 *atomicInt,
                    Types::Int64 value)
{
    atomicInt->d_value = value;
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::
    setInt64Release(typename AtomicTypes::Int64 *atomicInt,
                    Types::Int64 value)
{
    IMP::setInt64(atomicInt, value);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::
    swapInt64AcqRel(typename AtomicTypes::Int64 *atomicInt,
                    Types::Int64 swapValue)
{
    return IMP::swapInt64(atomicInt, swapValue);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::
    testAndSwapInt64AcqRel(typename AtomicTypes::Int64 *atomicInt,
                           Types::Int64 compareValue,
                           Types::Int64 swapValue)
{
    return IMP::testAndSwapInt64(atomicInt, compareValue, swapValue);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::
    addInt64NvRelaxed(typename AtomicTypes::Int64 *atomicInt,
                      Types::Int64 value)
{
    return IMP::addInt64NvAcqRel(atomicInt, value);
}

template <class IMP>
inline
Types::Int64 AtomicOperations_DefaultInt64<IMP>::
    addInt64NvAcqRel(typename AtomicTypes::Int64 *atomicInt,
                     Types::Int64 value)
{
    return IMP::addInt64Nv(atomicInt, value);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::
    addInt64(typename AtomicTypes::Int64 *atomicInt,
             Types::Int64 value)
{
    IMP::addInt64Nv(atomicInt, value);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::
    addInt64Relaxed(typename AtomicTypes::Int64 *atomicInt,
                    Types::Int64 value)
{
    IMP::addInt64NvRelaxed(atomicInt, value);
}

template <class IMP>
inline
void AtomicOperations_DefaultInt64<IMP>::
    addInt64AcqRel(typename AtomicTypes::Int64 *atomicInt,
                   Types::Int64 value)
{
    IMP::addInt64NvAcqRel(atomicInt, value);
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

                  // ----------------------------------------
                  // struct AtomicOperations_DefaultPointer32
                  // ----------------------------------------

// CLASS METHODS
template <class IMP>
inline
void AtomicOperations_DefaultPointer32<IMP>::
    initPointer(typename AtomicTypes::Pointer *atomicPtr,
                void *initialValue)
{
    atomicPtr->d_value = initialValue;
}

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
    getPtrRelaxed(typename AtomicTypes::Pointer const *atomicPtr)
{
    return reinterpret_cast<void *>(
             IMP::getIntRelaxed(
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
void AtomicOperations_DefaultPointer32<IMP>::
    setPtr(typename AtomicTypes::Pointer *atomicPtr,
           void *value)
{
    IMP::setInt(
        reinterpret_cast<typename AtomicTypes::Int *>(atomicPtr),
        reinterpret_cast<Types::IntPtr>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultPointer32<IMP>::
    setPtrRelaxed(typename AtomicTypes::Pointer *atomicPtr,
                  void *value)
{
    IMP::setIntRelaxed(
        reinterpret_cast<typename AtomicTypes::Int *>(atomicPtr),
        reinterpret_cast<Types::IntPtr>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultPointer32<IMP>::
    setPtrRelease(typename AtomicTypes::Pointer *atomicPtr,
                  void *value)
{
    IMP::setIntRelease(
        reinterpret_cast<typename AtomicTypes::Int *>(atomicPtr),
        reinterpret_cast<Types::IntPtr>(value));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer32<IMP>::
    swapPtr(typename AtomicTypes::Pointer *atomicPtr,
            void *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::swapInt(
                reinterpret_cast<typename AtomicTypes::Int *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer32<IMP>::
    swapPtrAcqRel(typename AtomicTypes::Pointer *atomicPtr,
                  void *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::swapIntAcqRel(
                reinterpret_cast<typename AtomicTypes::Int *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer32<IMP>::
    testAndSwapPtr(typename AtomicTypes::Pointer *atomicPtr,
                   void *compareValue,
                   void *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::testAndSwapInt(
                reinterpret_cast<typename AtomicTypes::Int *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(compareValue),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer32<IMP>::
    testAndSwapPtrAcqRel(typename AtomicTypes::Pointer *atomicPtr,
                         void *compareValue,
                         void *swapValue)
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
void AtomicOperations_DefaultPointer64<IMP>::
    initPointer(typename AtomicTypes::Pointer *atomicPtr,
                void *initialValue)
{
    atomicPtr->d_value = initialValue;
}

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
    getPtrRelaxed(typename AtomicTypes::Pointer const *atomicPtr)
{
    return reinterpret_cast<void *>(
             IMP::getInt64Relaxed(
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
void AtomicOperations_DefaultPointer64<IMP>::
    setPtr(typename AtomicTypes::Pointer *atomicPtr,
           void *value)
{
    IMP::setInt64(
        reinterpret_cast<typename AtomicTypes::Int64 *>(atomicPtr),
        reinterpret_cast<Types::IntPtr>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultPointer64<IMP>::
    setPtrRelaxed(typename AtomicTypes::Pointer *atomicPtr,
                  void *value)
{
    IMP::setInt64Relaxed(
        reinterpret_cast<typename AtomicTypes::Int64 *>(atomicPtr),
        reinterpret_cast<Types::IntPtr>(value));
}

template <class IMP>
inline
void AtomicOperations_DefaultPointer64<IMP>::
    setPtrRelease(typename AtomicTypes::Pointer *atomicPtr,
                  void *value)
{
    IMP::setInt64Release(
        reinterpret_cast<typename AtomicTypes::Int64 *>(atomicPtr),
        reinterpret_cast<Types::IntPtr>(value));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer64<IMP>::
    swapPtr(typename AtomicTypes::Pointer *atomicPtr,
            void *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::swapInt64(
                reinterpret_cast<typename AtomicTypes::Int64 *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer64<IMP>::
    swapPtrAcqRel(typename AtomicTypes::Pointer *atomicPtr,
                  void *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::swapInt64AcqRel(
                reinterpret_cast<typename AtomicTypes::Int64 *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer64<IMP>::
    testAndSwapPtr(typename AtomicTypes::Pointer   *atomicPtr,
                   void *compareValue,
                   void *swapValue)
{
    return reinterpret_cast<void *>(
            IMP::testAndSwapInt64(
                reinterpret_cast<typename AtomicTypes::Int64 *>(atomicPtr),
                reinterpret_cast<Types::IntPtr>(compareValue),
                reinterpret_cast<Types::IntPtr>(swapValue)));
}

template <class IMP>
inline
void *AtomicOperations_DefaultPointer64<IMP>::
    testAndSwapPtrAcqRel(typename AtomicTypes::Pointer *atomicPtr,
                         void *compareValue,
                         void *swapValue)
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
