// bsls_alignmentimp.h                                                -*-C++-*-
#ifndef INCLUDED_BSLS_ALIGNMENTIMP
#define INCLUDED_BSLS_ALIGNMENTIMP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementation meta-functions for alignment computation.
//
//@CLASSES:
//  bsls::AlignmentImpCalc: 'TYPE' parameter to alignment 'VALUE' map
//  bsls::AlignmentImpMatch: namespace for overloaded 'match' functions
//  bsls::AlignmentImpPriorityToType: 'PRIORITY' param to primitive type map
//  bsls::AlignmentImpTag: unique type of size 'SIZE' (parameter)
//
//@SEE_ALSO: bsls_alignmentfromtype, bsls_alignmenttotype, bsls_alignmentutil
//
//@DESCRIPTION: This component provides a suite of template meta-functions that
// can be used to compute (at compile-time) various platform-dependent
// alignment information.  The clients of this component are expected to be
// 'bsls' components such as 'bsls_alignmentfromtype', 'bsls_alignmenttotype',
// and 'bsls_alignmentutil'.  Other client code should use one of these 'bsls'
// components instead of using this component directly.
//
///Computing Alignment for a Type
///------------------------------
// The compiler alignment for a given type, 'T', can be computed by creating a
// structure containing a single 'char' member followed by a 'T' member:
//..
//  struct X {
//      char d_c;
//      T    d_t;
//  };
//..
// The compiler lays this structure out in memory as follows:
//..
//  +---+---+-------+
//  |d_c| P |  d_t  |
//  +---+---+-------+
//..
// where 'P' is padding added by the compiler to ensure that 'd_t' is properly
// aligned.  The alignment for 'T' is the number of bytes from the start of the
// structure to the beginning of 'd_t', which is also the total size of the
// structure minus the size of 'd_t':
//..
//  bsls::AlignmentImpCalc<T>::VALUE == sizeof(X) - sizeof(T);
//..
// Since 'sizeof' yields a compile-time constant, the alignment can be computed
// at compile time.
//
///Computing a Type Requiring an Alignment
///---------------------------------------
// A considerably more difficult compile-time computation supported by this
// component is that of determining a fundamental type with the same alignment
// requirements of a given type 'T'.  This involves computing the alignment for
// 'T', as above, and then performing an alignment-to-type lookup, all at
// compile time.  The general principles of this computation follow.
//
// We would like to create a template class that is specialized for each
// fundamental type's alignment.  Unfortunately, multiple types will have the
// same alignment and the compiler would issue a diagnostic if the same
// specialization was defined more than once.  To disambiguate, we create a
// "priority" class for each fundamental type that arbitrarily ranks that type
// relative to all of the other fundamental types.  Each priority class is
// derived from the next-lower priority class.  A set of overloaded functions
// are created such that, given two fundamental types with the same alignment,
// overload resolution will pick the one with the highest priority (i.e., the
// most-derived priority type).  The 'sizeof' operator and several template
// specializations are used to determine the compiler's choice of overloaded
// 'match' function.  The return value is mapped to a priority, which is, in
// turn, mapped to an appropriate primitive type.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: 'AlignmentImpCalc' Template
/// - - - - - - - - - - - - - - - - - - -
// Suppose that we want to write a program that needs to calculate the
// alignment requirements of both user-defined types and built-in types.
// Further suppose that the program will run on a platform where the alignment
// requirement of 'int' is 4 bytes.
//
// First, we define a 'struct', 'MyStruct', for which want to determine the
// alignment requirement:
//..
//  struct MyStruct {
//      char  d_c;
//      int   d_i;
//      short d_s;
//  };
//..
// Note that 'int' is the most alignment-demanding type within 'MyStruct'.
//
// Now, we use 'AlignmentImpCalc' to calculate the alignments of two
// types, 'short' and the 'MyStruct' we just defined:
//..
//  enum {
//      SHORT_ALIGNMENT     = bsls::AlignmentImpCalc<short   >::VALUE,
//      MY_STRUCT_ALIGNMENT = bsls::AlignmentImpCalc<MyStruct>::VALUE };
//..
// Finally, we observe the values of our alignments, we observe that
// the size of the 2 objects is a multiple of each object's alignment
// (which is true for all C++ types), and we observe that the size of
// 'MyStruct' is greater than its alignment.
//..
//  assert(2 == SHORT_ALIGNMENT);
//  assert(4 == MY_STRUCT_ALIGNMENT);
//
//  assert(0 == sizeof(short   ) % SHORT_ALIGNMENT);
//  assert(0 == sizeof(MyStruct) % MY_STRUCT_ALIGNMENT);
//
//  assert(sizeof(MyStruct) > MY_STRUCT_ALIGNMENT);
//..
//
///Example 2: Types Supporting 'AlignmentToType'
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose we to be able to determine a fundamental or pointer type that has
// both its size and alignment requirement equal to the alignment requirement
// of a specified template parameter type.  We can use the 'AlignmentImpTag'
// 'struct' template, the overloads of 'AlignmentImpMatch::match' class method,
// the 'AiignmentImp_Priority' template class, and the
// 'AlignmentImpPrioriityToType' template class to do this calculation.
//
// First, we define a class template, 'ConvertAlignmentToType', that provides a
// 'Type' alias to a fundamental or pointer type that has both its alignment
// requirement and size equal to the compile-time constant 'ALIGNMENT' 'int'
// parameter of the template.
//..
//  template <int ALIGNMENT>
//  struct ConvertAlignmentToType {
//      // This 'struct' provides a 'typedef', 'Type', that aliases a primitive
//      // type having the specified 'ALIGNMENT' requirement and size.
//
//    private:
//      // PRIVATE TYPES
//      typedef typename bsls::AlignmentImpMatch::MaxPriority MaxPriority;
//          // 'MaxPriority' is a typedef to the 'AlignmentImp_Priority'
//          // template class having the highest permissible priority value.
//
//      typedef          bsls::AlignmentImpTag<ALIGNMENT>     Tag;
//          // 'Tag' provides a typedef to the 'AlignmentImpTag' class
//          // configured with this 'struct's 'ALIGNMENT' parameter.
//
//      enum {
//          // Compute the priority of the primitive type corresponding to the
//          // specified 'ALIGNMENT'.  Many 'match' functions are declared, and
//          // at least one whose alignment and size fields are identical and
//          // equal to 'ALIGNMENT'.  Of those who match, the first match will
//          // be the one with the highest priority 'AlignmentImp_Priority'
//          // arg.
//
//          PRIORITY = sizeof(bsls::AlignmentImpMatch::match(Tag(),
//                                                           Tag(),
//                                                           MaxPriority()))
//      };
//
//    public:
//      // TYPES
//      typedef typename bsls::AlignmentImpPriorityToType<PRIORITY>::Type Type;
//          // Convert the 'PRIORITY' value we calculated back to a type that
//          // has the value 'ALIGNMENT' for both its alignment and it's size.
//  };
//..
// Then, we define two user defined types on which we will use
// 'ConvertAlignmentToType' on:
//..
//  struct MyStructA {
//      short  d_s;
//      double d_d;
//      int    d_i;
//  };
//
//  struct MyStructB {
//      double d_d[20];
//  };
//..
// Here, we calculate alignments for our 3 types with 'AlignmentImpCalc'.
//..
//  const int INT_ALIGNMENT = bsls::AlignmentImpCalc<int      >::VALUE;
//  const int A_ALIGNMENT   = bsls::AlignmentImpCalc<MyStructA>::VALUE;
//  const int B_ALIGNMENT   = bsls::AlignmentImpCalc<MyStructB>::VALUE;
//..
// Now, for each alignment requirement we just calculated, we utilize
// 'ConvertAlignmentToType' to determine the fundamental or pointer
// type having both size and alignment requirement equal to the
// calculated alignment requirement:
//..
//  typedef ConvertAlignmentToType<INT_ALIGNMENT>::Type IntAlignType;
//  typedef ConvertAlignmentToType<A_ALIGNMENT  >::Type ThisAlignType;
//  typedef ConvertAlignmentToType<B_ALIGNMENT  >::Type ThatAlignType;
//..
// Finally, we observe that the alignments of the '*AlignType's are the
// same as the alignments of the types from which they are derived, and that
// all the type determined by 'ConvertAlignmentToType' have sizes
// equal to their alignment requirements:
//..
//  assert(INT_ALIGNMENT == bsls::AlignmentImpCalc<IntAlignType >::VALUE);
//  assert(A_ALIGNMENT   == bsls::AlignmentImpCalc<ThisAlignType>::VALUE);
//  assert(B_ALIGNMENT   == bsls::AlignmentImpCalc<ThatAlignType>::VALUE);
//
//  assert(INT_ALIGNMENT == sizeof(IntAlignType));
//  assert(A_ALIGNMENT   == sizeof(ThisAlignType));
//  assert(B_ALIGNMENT   == sizeof(ThatAlignType));
//..

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

namespace bsls {

                // ======================
                // struct AlignmentImpTag
                // ======================

template <int SIZE>
struct AlignmentImpTag {
    // This 'struct' defines a unique type having the specified compile-time
    // 'SIZE'.

    // DATA
    char d_dummy[SIZE];
};

                // =======================
                // struct AlignmentImpCalc
                // =======================

template <class TYPE>
struct AlignmentImpCalc {
    // This 'struct' provides an enumerator 'VALUE' that is initialized to the
    // required alignment for the specified 'TYPE'.

  private:
    // PRIVATE TYPES
    struct AlignmentCalc {
        // This private 'struct' computes the required alignment for 'TYPE'.
        // The compiler inserts sufficient padding after the 'char' member so
        // that 'd_aligned' is correctly aligned for 'TYPE'.  The distance from
        // the start of the structure to 'd_aligned' is the alignment of
        // 'TYPE', and is computed as follows:
        //..
        //  sizeof(AlignmentImpCalc<TYPE>::AlignmentCalc) - sizeof(TYPE)
        //..

        // DATA
        char d_c;
        TYPE d_aligned;

      private:
        // NOT IMPLEMENTED
        AlignmentCalc();
        AlignmentCalc(const AlignmentCalc&);
        ~AlignmentCalc();
            // Prevent the compiler from automatically generating
            // default & copy constructors and destructor, as this could cause
            // problems if 'TYPE' has constructors / destructor that are
            // private or unimplemented.
    };

  public:
    // TYPES
    enum {
        // Define the compile-time computed alignment value for 'TYPE'.

        VALUE = sizeof(AlignmentCalc) - sizeof(TYPE)
    };

    typedef AlignmentImpTag<VALUE> Tag;
        // Alias for the unique type for each alignment value.
};

#if defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_OS_LINUX)
template <>
struct AlignmentImpCalc <long double> {
    // This 'struct' provides an enumerator 'VALUE' that is initialized to the
    // required alignment for long double on Linux on POWER.  This template
    // specialization is for long double on Linux on POWER where default malloc
    // in glibc returns memory aligned to 8-bytes, not 16-bytes.  8-byte
    // alignment is sufficient for proper long double operation on POWER even
    // though 16-byte alignment is more optimal (and is required for vector
    // instructions).
    //
    // Note: the optional tcmalloc library returns memory aligned to 16-bytes.

  public:
    // TYPES
    enum {
        // Define the alignment value for long double on Linux on POWER.

        VALUE = 8
    };

    typedef AlignmentImpTag<VALUE> Tag;
        // Alias for the unique type for each alignment value.
};
#endif

                // ===================================
                // struct AlignmentImp8ByteAlignedType
                // ===================================

#if defined(BSLS_PLATFORM_CPU_X86)                                            \
 && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
struct AlignmentImp8ByteAlignedType {
    // On Linux or Solaris x86, no natural type is aligned on an 8-byte
    // boundary, but we need such a type to implement low-level constructs
    // (e.g., 64-bit atomic types).

    long long d_dummy __attribute__((__aligned__(8)));
};
#endif

                // =================================
                // struct AlignmentImpPriorityToType
                // =================================

template <int PRIORITY>
struct AlignmentImpPriorityToType {
    // Specializations of this 'struct' provide a primitive type (as a 'Type'
    // 'typedef') that corresponds to the specified 'PRIORITY' level.
};

template <>
struct AlignmentImpPriorityToType< 1> {
    typedef long double Type;
};

template <>
struct AlignmentImpPriorityToType< 2> {
    typedef double      Type;
};

template <>
struct AlignmentImpPriorityToType< 3> {
    typedef float       Type;
};

template <>
struct AlignmentImpPriorityToType< 4> {
    typedef void      (*Type)();
};

template <>
struct AlignmentImpPriorityToType< 5> {
    typedef void       *Type;
};

template <>
struct AlignmentImpPriorityToType< 6> {
    typedef wchar_t     Type;
};

template <>
struct AlignmentImpPriorityToType< 7> {
    typedef bool        Type;
};

template <>
struct AlignmentImpPriorityToType< 8> {
    typedef long long   Type;
};

template <>
struct AlignmentImpPriorityToType< 9> {
    typedef long        Type;
};

template <>
struct AlignmentImpPriorityToType<10> {
    typedef int         Type;
};

template <>
struct AlignmentImpPriorityToType<11> {
    typedef short       Type;
};

template <>
struct AlignmentImpPriorityToType<12> {
    typedef char        Type;
};

#if defined(BSLS_PLATFORM_CPU_X86)                                            \
 && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
template <>
struct AlignmentImpPriorityToType<13> {
    typedef AlignmentImp8ByteAlignedType Type;
};
#endif

                // ============================
                // struct AlignmentImp_Priority
                // ============================

template <int VALUE>
struct AlignmentImp_Priority : AlignmentImp_Priority<VALUE - 1> {
    // This 'struct' provides a unique type that can be used as a trailing
    // function parameter for overloaded functions having otherwise identical
    // parameters.  The highest-priority overloaded function can be selected by
    // calling it with a high-priority argument.  Note that "highest priority"
    // means the largest 'VALUE' in this case.
};

template <>
struct AlignmentImp_Priority<1> {
    // Specialization of 'AlignmentImp_Priority' to terminate template
    // instantiation.
};

}  // close package namespace

namespace bsls {

    // Declare a 'match' function that is overloaded based on the alignment and
    // size of type 'T'.  The function has no implementation since it is used
    // only at compile-time to select the appropriate type for a given
    // alignment.  Return a tag that can used to look up a type using
    // 'AlignmentImpPriorityToType<P>::Type'.  Since multiple types can have
    // the same alignment and size, duplicate definitions are avoided by
    // overloading the function based on the priority 'P'.  When used, the
    // 'match' function with the highest priority is selected automatically.

                // ========================
                // struct AlignmentImpMatch
                // ========================

struct AlignmentImpMatch {
    // Namespace for a set of overloaded 'match' functions, as defined by the
    // macro 'BSLS_ALIGNMENTIMP_MATCH_FUNC'.

#   define BSLS_ALIGNMENTIMP_MATCH_FUNC(T, P)                                 \
           bsls::AlignmentImpTag<P> match(                                    \
                          bsls::AlignmentImpCalc<T>::Tag,                     \
                          bsls::AlignmentImpTag<static_cast<int>(sizeof(T))>, \
                          bsls::AlignmentImp_Priority<P>)

    // CLASS METHODS
    static BSLS_ALIGNMENTIMP_MATCH_FUNC(long double,                        1);
    static BSLS_ALIGNMENTIMP_MATCH_FUNC(double,                             2);
    static BSLS_ALIGNMENTIMP_MATCH_FUNC(float,                              3);
    static BSLS_ALIGNMENTIMP_MATCH_FUNC(void (*)(),                         4);
    static BSLS_ALIGNMENTIMP_MATCH_FUNC(void *,                             5);
    static BSLS_ALIGNMENTIMP_MATCH_FUNC(wchar_t,                            6);
    static BSLS_ALIGNMENTIMP_MATCH_FUNC(bool,                               7);
    static BSLS_ALIGNMENTIMP_MATCH_FUNC(long long,                          8);
    static BSLS_ALIGNMENTIMP_MATCH_FUNC(long,                               9);
    static BSLS_ALIGNMENTIMP_MATCH_FUNC(int,                               10);
    static BSLS_ALIGNMENTIMP_MATCH_FUNC(short,                             11);
    static BSLS_ALIGNMENTIMP_MATCH_FUNC(char,                              12);
        // This function will match a type with the size and alignment the size
        // of the type of the first macro argument, and return an object whose
        // size is the 2nd argument of the macro.

#if defined(BSLS_PLATFORM_CPU_X86)                                            \
 && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
        // This type exists, and is needed, only on Linux

    static BSLS_ALIGNMENTIMP_MATCH_FUNC(AlignmentImp8ByteAlignedType,      13);
#endif

    typedef AlignmentImp_Priority<13> MaxPriority;
};

}  // close package namespace

#undef BSLS_ALIGNMENTIMP_MATCH_FUNC


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
