// bdes_int64.h                                                       -*-C++-*-
#ifndef INCLUDED_BDES_INT64
#define INCLUDED_BDES_INT64

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a header file to satisfy old redundant #includes.
//
//@DEPRECATED: This component should not be used in new code, and will be
// deleted in the near future.  Please see 'bdef_function', 'bdef_bind', etc.
// for alternatives that should be used for all future development.
//
//@DESCRIPTION: This component has been deleted, but there remains legacy code
// still including this header, without relying on its contents.  To avoid
// immediately breaking that code, we retain this effectively empty header for
// another BDE release cycle, while notifying the owners of the legacy code.
//
///USAGE
///-----
// Do not use this component; use 'bsls_types' instead.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  // required by 'bdes'
#endif

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

// Transitive inclusions may be depended on by some Robo code.

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#endif // BDE_OMIT_INTERNAL_DEPRECATED

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
