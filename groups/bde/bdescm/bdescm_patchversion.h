// bdescm_patchversion.h                                              -*-C++-*-
#ifndef INCLUDED_BDESCM_PATCHVERSION
#define INCLUDED_BDESCM_PATCHVERSION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif

BDES_IDENT("$Id: $")

//@PURPOSE: Provide common patch versioning information.
//
//@SEE_ALSO: bdescm_version, bdescm_versiontag
//
//@AUTHOR: Mike Giroux (mgiroux)
//
//@DESCRIPTION: This component provides patch versioning information for the
// 'bde' package group (and any other package groups released atomically with
// 'bde').  The 'BDESCM_PATCHVERSION_PATCH' macro this component defines is of
// limited standalone utility.
//
// This header should only be included by implementation files, to avoid
// unnecessary recompilations of client code.
//
///Usage
///-----
// At compile time, the 'patch' version of 'bde' can be examined.
//..
//  #if BDESCM_PATCHVERSION_PATCH > 0
//      printf("This is a patch release\n");
//  #else
//      printf("This is a \".0\" initial release\n");
//  #endif
//..

#define BDESCM_PATCHVERSION_PATCH 3
    // Provide the patch version number of the current (latest) BDE release.

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
