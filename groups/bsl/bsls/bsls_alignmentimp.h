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
//  bsls::AlignmentImpCalc: map 'TYPE' parameter to alignment 'VALUE'
//  bsls::AlignmentImpMatch: namespace for overloaded 'match' functions
//  bsls::AlignmentImpPriorityToType: map 'PRIORITY' param to primitive type
//  bsls::AlignmentImpTag: unique type of size 'SIZE' (parameter)
//
//@SEE_ALSO: bsls_alignmentfromtype, bsls_alignmenttotype, bsls_alignmentutil
//
//@AUTHOR: Pablo Halpern (phalpern), Rohan Bhindwale (rbhindwa)
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
// Since this component is not meant to be used directly by client code, a
// usage example is not provided.  See 'bsls_alignmentfromtype' and
// 'bsls_alignmenttotype' for examples of how to use the facilities provided by
// this component.

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

template <typename TYPE>
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
            // Prevent the compiler from automatically generating
            // default & copy constructors, as this could cause problems if
            // 'TYPE' has constructors that are private or unimplmented.
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

                // ===================================
                // struct AlignmentImp8ByteAlignedType
                // ===================================

struct AlignmentImp8ByteAlignedType {
# if defined(BSLS_PLATFORM__CPU_X86) && defined(BSLS_PLATFORM__CMP_GNU)
    // On Linux x86, no natural type is aligned on an 8-byte boundary, but we
    // need such a type to implement low-level constructs (e.g., 64-bit atomic
    // types).

    long long d_dummy __attribute__((__aligned__(8)));
# else
    double d_dummy;
# endif
};

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

template <>
struct AlignmentImpPriorityToType<13> {
    typedef AlignmentImp8ByteAlignedType Type;
};

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

#   define BSLS_ALIGNMENTIMP_MATCH_FUNC(T, P)                               \
           bsls::AlignmentImpTag<P> match(bsls::AlignmentImpCalc<T>::Tag,   \
                                          bsls::AlignmentImpTag<sizeof(T)>, \
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
    static BSLS_ALIGNMENTIMP_MATCH_FUNC(AlignmentImp8ByteAlignedType,      13);
        // This function will match a type with the size and alignment the size
        // of the type of the first macro argument, and return an object whose
        // size is the 2nd argument of the macro.

    typedef AlignmentImp_Priority<13> MaxPriority;
};

}  // close package namespace

#undef BSLS_ALIGNMENTIMP_MATCH_FUNC

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#if defined(BSLS_PLATFORM__CPU_X86) && defined(BSLS_PLATFORM__CMP_GNU)
typedef bsls::AlignmentImp8ByteAlignedType bsls_AlignmentImp8ByteAlignedType;
    // This alias is defined for backward compatibility.
#endif

#ifdef bsls_AlignmentImpTag
#undef bsls_AlignmentImpTag
#endif
#define bsls_AlignmentImpTag bsls::AlignmentImpTag
    // This alias is defined for backward compatibility.

#ifdef bsls_AlignmentImpPriorityToType
#undef bsls_AlignmentImpPriorityToType
#endif
#define bsls_AlignmentImpPriorityToType bsls::AlignmentImpPriorityToType
    // This alias is defined for backward compatibility.

typedef bsls::AlignmentImpMatch bsls_AlignmentImpMatch;
    // This alias is defined for backward compatibility.

#ifdef bsls_AlignmentImpCalc
#undef bsls_AlignmentImpCalc
#endif
#define bsls_AlignmentImpCalc bsls::AlignmentImpCalc
    // This alias is defined for backward compatibility.

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
