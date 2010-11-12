// bdetst_stlcontainers.h                  -*-C++-*-
#ifndef INCLUDED_BDETST_STLCONTAINERS
#define INCLUDED_BDETST_STLCONTAINERS

//@PURPOSE: Test various features of STL containers
//
//@CLASSES:
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:
//
///USAGE
///-----
//

// #includes needed by test driver
#ifndef INCLUDED_BDEMA_TESTALLOCATOR
#include <bdema_testallocator.h>
#endif

#ifndef INCLUDED_BDEMA_DEFAULT
#include <bdema_default.h>
#endif

// Indirect includes are explicitly listed here to force rebuild on changes:
#ifndef INCLUDED_BDESTL_VECTOR
#include <bdestl_vector.h>
#endif

#ifndef INCLUDED_BDESTL_DEQUE
#include <bdestl_deque.h>
#endif

#ifndef INCLUDED_BDESTL_UTIL
#include <bdestl_util.h>
#endif

#ifndef INCLUDED_BDEALG_SCALARPRIMITIVES
#include <bdealg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BDEALG_ARRAYPRIMITIVES
#include <bdealg_arrayprimitives.h>
#endif

#ifndef INCLUDED_BDEALG_TYPETRAITS
#include <bdealg_typetraits.h>
#endif

#ifndef INCLUDED_STD_STRING
#include <string>
#define INCLUDED_STD_STRING
#endif

#ifndef INCLUDED_STD_VECTOR
#include <vector>
#define INCLUDED_STD_VECTOR
#endif

#ifndef INCLUDED_STD_DEQUE
#include <deque>
#define INCLUDED_STD_DEQUE
#endif

#ifndef INCLUDED_STD_LIST
#include <list>
#define INCLUDED_STD_LIST
#endif

#ifndef INCLUDED_STD_SET
#include <set>
#define INCLUDED_STD_SET
#endif

#ifndef INCLUDED_STD_MAP
#include <map>
#define INCLUDED_STD_MAP
#endif

#ifndef INCLUDED_STD_HASH_SET
#include <hash_set>
#define INCLUDED_STD_HASH_SET
#endif

#ifndef INCLUDED_STD_HASH_MAP
#include <hash_map>
#define INCLUDED_STD_HASH_MAP
#endif


#endif // ! defined(INCLUDED_BDETST_STLCONTAINERS)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
