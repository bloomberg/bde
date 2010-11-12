// bsls_ident.h                 -*-C++-*-
#ifndef INCLUDED_BSLS_IDENT
#define INCLUDED_BSLS_IDENT

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
// Include 'bsls_ident.h' and use the BSLS_IDENT macro.  For header files this
// should be done directly after the include guards, e.g., bsls_somefile.h:
//..
//  // bsls_somefile.h            -*-C++-*-
//  #ifndef INCLUDED_BSLS_SOMEFILE
//  #define INCLUDED_BSLS_SOMEFILE
//
//  #include <bsls_ident.h>
//  BSLS_IDENT("$Id: $")
//
//  // ...
//
//  #endif // INCLUDED_BSLS_SOMEFILE
//..
// For cpp files it should be done directly after the comment for the file name
// and the language, e.g., bsls_somefile.cpp:
//..
//  // bsls_ident.t.cpp           -*-C++-*-
//
//  #include <bsls_ident.h>
//  BSLS_IDENT("$Id: $")
//..

#if 0 // TBD, enable this for next release
#if defined(__GNUC__)
  #if !defined(_AIX)
    /* does not work with AIX as; might work with GNU as (gas) (not tested) */
    #define _BSLS_IDENT(str) asm(#str);
    #define BSLS_IDENT(str) _BSLS_IDENT(.ident str)
  #else
    #define BSLS_IDENT(str)
  #endif
#elif defined(_AIX)
  #define _BSLS_IDENT(str) _Pragma(#str)
  #define BSLS_IDENT(str) _BSLS_IDENT(comment (user, str))
#elif defined(__sun)
  #define _BSLS_IDENT(str) _Pragma(#str)
  #define BSLS_IDENT(str) _BSLS_IDENT(ident str)
#else
  #define BSLS_IDENT(str)
#endif
#else // TBD: Expand to nothing for now
  #define BSLS_IDENT(str)
#endif

// Unlike most headers, which use 'BSLS_IDENT' as described in the usage, this
// header must wait till 'BSLS_IDENT' has been define.
BSLS_IDENT("$Id: $")

#endif // INCLUDED_BSLS_IDENT

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

