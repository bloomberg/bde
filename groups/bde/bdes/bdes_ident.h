// bdes_ident.h                                                       -*-C++-*-
#ifndef INCLUDED_BDES_IDENT
#define INCLUDED_BDES_IDENT

//@PURPOSE: Provide macros for inserting SCM Ids into source files.
//
//@CLASSES:
//
//@AUTHOR: Anthony Comerico (acomeric)
//
//@DESCRIPTION: The purpose of this component is to provide macros for
// inserting SCM Ids into source files.
//
///Usage
///-----
// Include 'bdes_ident.h' and use the 'BDES_IDENT' macro or the
// 'BDES_IDENT_RCSID' macro.  For header files, this should be done directly
// after the include guards, e.g., in 'bdes_somefile.h':
//..
//  // bdes_somefile.h            -*-C++-*-
//  #ifndef INCLUDED_BDES_SOMEFILE
//  #define INCLUDED_BDES_SOMEFILE
//
//  #ifndef INCLUDED_BDES_IDENT
//  #include <bdes_ident.h>
//  #endif
//  BDES_IDENT("$Id: $")
//
//  //@PURPOSE: Provide an example of 'bdes_ident.h' usage.
//
//  // ...
//
//  #endif // INCLUDED_BDES_SOMEFILE
//..
// For 'cpp' files, the following pattern should be used:
//..
//  // bdes_somefile.cpp          -*-C++-*-
//  #include <bdes_somefile.h>
//
//  #include <bdes_ident.h>
//  BDES_IDENT_RCSID(bdes_somefile_cpp,"$Id$ $CSID$")
//
//  // ... other includes ...
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  // required by 'bdes'
#endif

#ifdef BDES_IDENT_OFF


#define BDES_IDENT(str)
#define BDES_IDENT_RCSID(tag,str)


#else /* !BDES_IDENT_OFF */


/* BDES_IDENT() - insert string into .comment binary segment (if supported)*/

#if defined(__GNUC__)
  #if !defined(_AIX)
    /* does not work with AIX as; might work with GNU as (gas) (not tested) */
    /* gcc has no pragma equivalent for #ident */
    #define _BDES_IDENT(str) __asm__(#str);
    #define BDES_IDENT(str) _BDES_IDENT(.ident str)
  #endif
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
  /* Sun Studio CC does not support _Pragma() until Sun Studio 12 */
  #if (!defined(__cplusplus)) || (__SUNPRO_CC >= 0x590)
    #define _BDES_IDENT(str) _Pragma(#str)
    #define BDES_IDENT(str) _BDES_IDENT(ident str)
  #endif
#elif defined(__IBMC__) || defined(__IBMCPP__)
  #define _BDES_IDENT(str) _Pragma(#str)
  #define BDES_IDENT(str) _BDES_IDENT(comment (user, str))
#elif defined(_MSC_VER) /* Microsoft Visual Studio Compiler */
  #define BDES_IDENT(str)
#elif defined(__HP_cc) || defined(__HP_aCC)
  #define _BDES_IDENT(str) _Pragma(#str)
  #define BDES_IDENT(str) _BDES_IDENT(versionid str)
#endif

#ifndef BDES_IDENT
#define BDES_IDENT(str)
#endif


/* multi-level indirection to force macro expansion before concatenation */
/* (order of concatenation/macro expansion unspecified in ANSI standard) */
#define BDES_IDENT_JOIN2(x,y) x ## y
#define BDES_IDENT_JOIN(x,y) BDES_IDENT_JOIN2(x,y)


/* BDES_IDENT_RCSID() - insert ident str (specific to platform/compiler) */

#if defined(__GNUC__)
  #if !defined(_AIX)
    #define BDES_IDENT_RCSID(tag,str) BDES_IDENT(str)
  #else /* _AIX */
    // (XXX: look into further if we ever use gcc to build prod tasks on AIX)
    #ifndef lint
    #define BDES_IDENT_RCSID(tag,str) \
      static char BDES_IDENT_JOIN(ident_,tag)[] __attribute__((__unused__)) \
                  = str;
    #endif
  #endif
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
  #if (!defined(__cplusplus)) || (__SUNPRO_CC >= 0x590)
    #define BDES_IDENT_RCSID(tag,str) BDES_IDENT(str)
  #else /* Sun Studio CC does not support _Pragma() until Sun Studio 12 */
    #ifndef lint
    #define BDES_IDENT_RCSID(tag,str) \
      static char BDES_IDENT_JOIN(ident_,tag)[] = str;
    #endif
  #endif
#elif defined(__IBMC__) || defined(__IBMCPP__)
  #if (defined(__IBMC__)   && __IBMC__   >= 1010) \
    ||(defined(__IBMCPP__) && __IBMCPP__ >= 1010)
    #define BDES_IDENT_RCSID(tag,str) BDES_IDENT(str)
  #else
    /* Early versions of IBM xlc did not preserve .comment in binary
     * executables or pre-linked libraries.  Fixed by IBM in following
     * releases:
     * xlC v8.0 in May 2008 PTF (8.0.0.19) with -qxflag=new_pragma_comment_user
     * xlC v9.0 in July 2008 PTF (9.0.0.5)
     * xlC v10.1 GA
     * Enabled above only for xlC 10.1 or later (which can be detected
     * reliably) (Use C compiler and printf("%s\n", __xlc__) to see x.x.x.x
     * version, but strings are not comparable in macros)
     */
    /* various ways to create C string, including const, volatile, more */
    /* static char instead of static const char with xlC -qnoro -qnoroconst */
    /* xlC might end up optimizing this away, anyway */
    #ifndef lint
    #define BDES_IDENT_RCSID(tag,str) \
      static char BDES_IDENT_JOIN(ident_,tag)[] = str;
    #endif
  #endif
#elif defined(_MSC_VER)
  #define BDES_IDENT_RCSID(tag,str)
#elif defined(__HP_cc) || defined(__HP_aCC)
  #define BDES_IDENT_RCSID(tag,str) BDES_IDENT(str)
#else
  /* others: add conditions above for compilers able to support BDES_IDENT */
  #ifndef lint
  #define BDES_IDENT_RCSID(tag,str) \
    static char BDES_IDENT_JOIN(ident_,tag)[] = str;
  #endif
#endif

#ifndef BDES_IDENT_RCSID
#define BDES_IDENT_RCSID(tag,str)
#endif

#endif /* !BDES_IDENT_OFF */


#if defined(__GNUC__)
  #define BDES_IDENT_PRAGMA_ONCE _Pragma("once")
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
  /* Sun Studio does not support #pragma once.  Instead, it natively detects if
   * the entire non-comment portion of a file is wrapped by #ifndef, and if so,
   * optimizes away reopens of the file if the #ifndef condition is false */
  #define BDES_IDENT_PRAGMA_ONCE
#elif defined(__IBMC__) || defined(__IBMCPP__)
  #define BDES_IDENT_PRAGMA_ONCE _Pragma("once")
#elif defined(_MSC_VER)
  #define BDES_IDENT_PRAGMA_ONCE
#elif defined(__HP_cc) || defined(__HP_aCC)
  /* HP-UX ANSI C compiler does not document support for #pragma once,
   * but does document that unknown pragmas are ignored */
  /* UPDATE: aCC A.06.20 fails on this, so disable for now and revisit later */
  #define BDES_IDENT_PRAGMA_ONCE /*_Pragma("once")*/
#else
  #define BDES_IDENT_PRAGMA_ONCE _Pragma("once")
#endif
BDES_IDENT_PRAGMA_ONCE




#endif // INCLUDED_BDES_IDENT

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
