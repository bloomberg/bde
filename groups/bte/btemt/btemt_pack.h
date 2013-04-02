// btemt_pack.h               -*-C++-*-
#ifndef INCLUDED_BTEMT_PACK
#define INCLUDED_BTEMT_PACK

//@PURPOSE: Provide macros to set class byte alignment independent of compiler
//
//@CLASSES:
//
//@AUTHOR: John Davison (jdavison)
//         Christopher Palmer (cpalmer)
//
//@SEE ALSO:
//
//@DESCRIPTION: 
// This header file asks the compiler to pack "struct" types using 1-byte
// alignment rather than the compiler's default.  This assumes packing may
// be done on this compiler via a #pragma, but that the type of pragma is
// compiler specific.  This component defines several macros that will set the
// compiling compiler's options.
//
// Alternative structure packing is, for the most part, an idiom that should be
// discouraged.  Application developers are strongly encouraged to use
// this header pairs as sparingly, and as locally, as possible.
//
///Usage Example
///-------------
// The following example shows how to create struct and specify that 1-byte
// alignment should be used over the compiler's default packing:
//..
//  #pragma BTEMT_PACK1
//  struct Foo {
//      unsigned char d_data1;
//      int           d_data2;
//  };
//  #pragma BTEMT_UNPACK
//..
// Now we will assert that indeed the struct is aligned on 1-byte boundaries:
//..
//  assert(sizeof(Foo) == sizeof(unsigned char) + sizeof(int));
//..

#ifdef _HP_UX

// "NOTE: The HP_ALIGN pragma, which is supported by the HP ANSI C compiler,
// is not supported by aCC. The pack and unalign pragmas can replace most of
// the HP_ALIGN functionality" [2].
//
// One is well advised to consult [1] to learn about the funny implications of
// the #pragma pack directive on typedefs, at least when you're using the C
// compiler.  Blood-curdling stuff.
//
// "NOTE: The basic scalar types, array types, enumeration types, and pointer
// types are not affected by the HP_ALIGN pragma. The pragma only affects
// struct or union types and typedefsno other types are affected by specifying
// the HP_ALIGN pragma."
//
// More HP_ALIGN weirdness, perhaps only of historical interest.  Pay
// particular attention to what they say about typedefs:
//
// "The key to understanding HP_ALIGN is the following concept: typedefs
// and struct or union types retain their original alignment mode throughout
// the entire file. Therefore, when a type with one alignment is used in a
// different alignment mode, it will still keep its original alignment."
// See the "Example 1: Using Typedefs" example in the HP_ALIGN documentation.
// However, note also that we shouldn't be using HP_ALIGN in the first place;
// it is unavailable from HP's C++ compiler.
//
// Also: "The result of applying #pragma pack n to constructs other than class
// definitions (including struct definitions) is undefined and not
// supported." [2, p. 127 of the PDF file].  I take this to mean that a
// typedef of a packed struct is the same whether or not you do it within a
// pack-directive.  (Otherwise the semantics would just be too complicated.)
//
// "HP aCC supports misaligned data access using the unalign pragma. The
// unalign pragma can be applied on typedef to define a type with special
// alignment. The unalign pragma takes effect only on next declaration."
//
// My take on this is that, as long as you declare a struct (or class) within
// the context of a certain "pack" pragma, the associated alignment requirement//
// extends to the typedef, which means that there's no particular need to
// perform the typedef within a #pragma.  On the other hand, what if one
// creates a typedef that refers to an incomplete type?  Because it is
// incomplete, its alignment requirements are unknown, and consequently that
// aspect of the typedef is unknown as well.  To wit:
//
//     /*
//     // This pointer type has unknown alignment requirements (for its
//     // referent type).
//     ///
//     typedef struct NotYetDefined//NydPtr;
//
// This reminds me of the "Forward declaration of enum" subject that was
// discussed on USENET newsgroup "comp.std.c++" in 2003.  On 2003-02-26,
// Francis Glassborow wrote:
//
// 1) Forward declaration only allows return by pointer or reference.
// 2) For efficiency reasons enums can be of different sizes with different
// underlying integral types.
// 3) For efficiency reasons pointers to types of different sizes are not
// only allowed to be different (in size and layout) but are actually
// different on existing architectures.
//
// If I'm reading this correctly, the issues that Francis Glassborow raises
// with forward-declared enums are exactly those raised by structs on a
// platform that allows you to influence their alignment requirements via a
// #pack pragma at the time of definition.  However, banning the usage of
// pointers to forward-declared structs would violate the Standard, so it seems
// to me that the only solution for such platforms is to use the same pointer
// representations for all structs, period.
//
// The conclusions I've drawn from this is:
//
// 1. Keep "#pragma pack" blocks as tight around individual struct definitions
//    as possible (except for the typedef mentioned in #4); consider them as
//    struct qualifiers more than anything else.
//
// 2. Prefer "#pragma pack" to HP_ALIGN.  (HP does not support HP_ALIGN for
//    C++.)
//
// 3. A compilation environment that allows you to specify the alignment of any
//    struct via a #pragma must impose the most permissive "representation and
//    alignment requirements" allowed.  Otherwise, you couldn't use pointers to
//    forward-declared structs (i.e. structs that are incomplete types.
//    (Either that, or you'd have to have some funny rules about the
//    conditions under which one could legally forward-declare a struct.
//    I've never encountered anyone even//considering* the latter.)
//
// 4. You're probably OK with the "typedef struct foo {...} foo" idiom inside a
//    "#pragma pack" block, because both the struct and its typedef will have
//    the same alignment.  For extra safety, you could define the typedef
//    outside of the "#pragma pack" block too, but it breaks the idiom.  I'd
//    rather continue using the idiom, which is practically as old as C itself.
//
// References:
//
//     1. "HP C/HP-UX Programmer's Guide: HP 9000 Computers" --> "Chapter 2
//        Storage and Alignment Comparisons".
//        URL: http://docs.hp.com/en/B3901-90005/ch02.html
//        URL: http://docs.hp.com/en/B3901-90005/B3901-90005.pdf
//
//     2. "HP aC++/HP C Programmer's Guide: Version A.06.15" --> "Chapter 3
//        Pragma Directives and Attributes" --> "Data Alignment Pragmas".
//        URL: http://docs.hp.com/en/B3901-90024/ch03s03.html
//        URL: http://docs.hp.com/en/B3901-90024/B3901-90024.pdf
//
//     3. Francis Glassborow, USENET Newsgroup "comp.std.c++", Subject: "Re:
//        Forward declaration of enum", posted Feb 26 2003, 7:38 pm.
//
//         -- jmd, Sat Oct  6 11:34:45 EDT 2007

#define BTEMT_PACK1  pack 1
#define BTEMT_UNPACK pack

#elif defined(_AIX)

// Reference:
//
//     1. "IBM AIX Compiler Information Center" --> "XL C/C++ V7.0 (for AIX)"
//        --> "Compiler reference" --> "Compiler Options" --> "General Purpose
//        Pragmas" --> "#pragma align"
//        URL: http://publib.boulder.ibm.com/infocenter/comphelp/v7v91/
//                  index.jsp
//
//     2. "IBM AIX Compiler Information Center" --> "XL C/C++ V7.0 (for AIX)"
//        --> "Compiler reference" --> "Compiler Options" --> "General Purpose
//        Pragmas" --> "#pragma pack"
//        URL: http://publib.boulder.ibm.com/infocenter/comphelp/v7v91/
//                  index.jsp
//
//         -- jmd, Sat Oct  6 11:53:04 EDT 2007

#define BTEMT_PACK1  pack(1)
#define BTEMT_UNPACK pack(pop)

#elif defined(__sun) && (defined(__SUNPRO_C) || defined(__SUNPRO_CC))

// Reference:
//
//     1. "C User's Guide" --> "Chapter 3" --> "Sun ANSI/ISO C
//        Compiler-Specific Information".
//        URL: http://docs.sun.com/source/806-3567/sun.html
//        NOTE: this corresponds to an unspecified version of the
//         standard-issue "cc" C compiler that is shipped with Solaris.

#define BTEMT_PACK1  pack(1)
#define BTEMT_UNPACK pack()

#elif defined(_MSC_VER)
#    if ! (defined(lint) || defined(RC_INVOKED))
#        if ( _MSC_VER >= 800 && !defined(_M_I86)) \
			|| defined(_PUSHPOP_SUPPORTED)
#pragma warning(disable:4103)
#            if !(defined( MIDL_PASS )) || defined( __midl )

#define BTEMT_PACK1  pack(push,1)
#define BTEMT_UNPACK pack(pop)

#            else

#define BTEMT_PACK1  pack(1)
#define BTEMT_UNPACK pack()

#            endif
#        else

#define BTEMT_PACK1  pack(1)
#define BTEMT_UNPACK pack()

#        endif
#    endif // ! (defined(lint) || defined(RC_INVOKED))
#elif defined(__GNUC__)

// Reference:
//  _Using the GNU Compiler Collection_, Free Software Foundation,
//  Section 6.58.8 "Structure-Packing Pragmas",
//  http://gcc.gnu.org/onlinedocs/gcc/Structure_002dPacking-Pragmas.html,
//  retrieved 2013-04-02.
#define BTEMT_PACK1  pack(push,1)
#define BTEMT_UNPACK pack(pop)
#else
#    error unrecognized compiler.
	typedef const char BtemtPackHUnrecognizedCompiler[-1];
#endif /* _UNIX */

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
