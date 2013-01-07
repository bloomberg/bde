/* bdes_ident.h                                                      -*-C-*- */
#ifndef INCLUDED_BDES_IDENT
#define INCLUDED_BDES_IDENT

/*@PURPOSE: Provide macros for inserting SCM Ids into source files.
 *
 *@CLASSES:
 *
 *@AUTHOR: Anthony Comerico (acomeric), Mike Giroux (mgiroux)
 *
 *@DESCRIPTION: The purpose of this component is to provide macros for
 * inserting SCM Ids into source files.
 *
 **Usage
 **-----
 * Include 'bdes_ident.h' and use the 'BDES_IDENT' macro or the
 * 'BDES_IDENT_RCSID' macro.  For header files, this should be done directly
 * after the include guards, e.g., in 'bdes_somefile.h':
 *..
 *  // bdes_somefile.h            -*-C++-*-
 *  #ifndef INCLUDED_BDES_SOMEFILE
 *  #define INCLUDED_BDES_SOMEFILE
 *
 *  #ifndef INCLUDED_BDES_IDENT
 *  #include <bdes_ident.h>
 *  #endif
 *  BDES_IDENT("$Id: $")
 *
 *  //@PURPOSE: Provide an example of 'bdes_ident.h' usage.
 *
 *  // ...
 *
 *  #endif // INCLUDED_BDES_SOMEFILE
 *..
 * For 'cpp' files, the following pattern should be used:
 *..
 *  // bdes_somefile.cpp          -*-C++-*-
 *  #include <bdes_somefile.h>
 *
 *  #include <bdes_ident.h>
 *  BDES_IDENT_RCSID(bdes_somefile_cpp,"$Id$ $CSID$")
 *
 *  // ... other includes ...
 *..
 * For 'c' files, the following pattern should be used (with C comments if
 * the 'c' compiler doesn't support C++ comments, which are used here to avoid
 * nesting):
 *..
 *  // bdes_somefile.c          -*-C-*-
 *  #include <bdes_somefile.h>
 *
 *  #include <bdes_ident.h>
 *  BDES_IDENT_RCSID(bdes_somefile_c,"$Id$ $CSID$")
 *
 *  // ... other includes ...
 *..
 */

/* Include 'bslscm_version.h' only if included from 'C++' code. */
#ifdef __cplusplus
#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  /* required by 'bdes' */
#endif
#endif /* __cplusplus */

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif

#ifdef BDES_IDENT_OFF
    #define BDES_IDENT(str)
    #define BDES_IDENT_RCSID(tag,str)
#else /* !BDES_IDENT_OFF */
    // Use the bsls_ident definitions for all macros.
    #define BDES_IDENT(str)           BSLS_IDENT(str)
    #define BDES_IDENT_RCSID(tag,str) BSLS_IDENT_RCSID(tag,str)
#endif /* !BDES_IDENT_OFF */

#define BDES_IDENT_PRAGMA_ONCE    BSLS_IDENT_PRAGMA_ONCE

BDES_IDENT_PRAGMA_ONCE

#endif /* INCLUDED_BDES_IDENT */

/* ---------------------------------------------------------------------------
 * NOTICE:
 *      Copyright (C) Bloomberg L.P., 2011
 *      All Rights Reserved.
 *      Property of Bloomberg L.P. (BLP)
 *      This software is made available solely pursuant to the
 *      terms of a BLP license agreement which governs its use.
 * ----------------------------- END-OF-FILE --------------------------------*/
