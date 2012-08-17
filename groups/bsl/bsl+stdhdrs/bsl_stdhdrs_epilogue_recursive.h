// bsl_stdhdrs_epilogue_recursive.h                                   -*-C++-*-

//@PURPOSE: Provide corresponding bslstl definitions for C++ standard headers.
//
//@SEE_ALSO: bsl_stdhdrs_epilogue, bsl+stdhdrs.txt
//
//@AUTHOR: Pablo Halpern (phalpern), Arthur Chiu (achiu21),
//         Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This header is the recursive part of the implementation of
// 'bsl_stdhdrs_epilogue.h'.  Its purpose is to include appropriate 'bslstp'
// and 'bslstl' definitions for each of the native STL headers found in the
// inclusion chain up to this point.  Sometimes included 'bslstl' headers may,
// in turn, include additional native STL headers, in which case this header
// needs to be processed again for those new STL headers.  In order to
// accomplish this task, this header includes itself recursively until no more
// new native STL headers are found.  This is the reason for the '_recursive'
// suffix in the header name.
//
// After each new 'bslstp' or 'bslstl' header is included we define the macro
// 'INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE' which indicates that this recursive
// epilogue needs to be included again to process any new native STL headers
// that may have been included.  When no new native STL headers are left to be
// processed, 'INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE' will not be defined,
// which stops the recursion.
//
// Note that the order of 'bslstp' and 'bslstl' headers in this file is in the
// reverse order of dependencies between each other.  Although the epilogue is
// designed to work with any order, doing so minimizes the number of times the
// epilogue is included recursively.
//
// Note that there are no include guards.  THIS FILE IS INTENDED FOR MULTIPLE
// INCLUSION.

// Prevent accidental inclusion from anywhere but 'bsl_stdhdrs_epilogue.h'.
#if !defined(USE_BSL_STDHDRS_EPILOGUE_RECURSIVE)
#error "this header can only be included from bsl_stdhdrs_epilogue.h"
#endif

// 'INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE' controls the recursive inclusion,
// so it should not be leaked outside of this header.  Doing so may cause an
// infinite recursive inclusion.
#if defined(INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE)
#error "potential infinite inclusion of the recursive epilogue detected"
#endif

// First start with STLPort containers, which may include native headers.  The
// 'BSL_INCLUDE_*' macros are defined in the standard headers inside the
// 'bsl+stdhdrs' package.

#ifdef BSL_INCLUDE_BSL_LIST
# ifndef INCLUDED_BSLSTP_LIST
#   include <bslstp_list.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_QUEUE
# ifndef INCLUDED_BSLSTP_QUEUE
#   include <bslstp_queue.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_STACK
# ifndef INCLUDED_BSLSTP_STACK
#   include <bslstp_stack.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_HASH_MAP
# ifndef INCLUDED_BSLSTP_HASHMAP
#   include <bslstp_hashmap.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_HASH_SET
# ifndef INCLUDED_BSLSTP_HASHSET
#   include <bslstp_hashset.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_SLIST
# ifndef INCLUDED_BSLSTP_SLIST
#   include <bslstp_slist.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_SSTREAM
# ifndef INCLUDED_BSLSTP_SSTREAM
#   include <bslstp_sstream.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_IOSFWD
# ifndef INCLUDED_BSLSTP_IOSFWD
#   include <bslstp_iosfwd.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

// Now include those 'bslstl' components corresponding to the 'bsl+stdhdrs'
// files that have been included.

// If '<algorithm>' was included, then include SGI extension to algorithms.
#ifdef INCLUDED_NATIVE_ALGORITHM
# ifndef INCLUDED_BSLSTP_EXALGORITHM
#   include <bslstp_exalgorithm.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

// 'bslstl' containers go here.

#ifdef BSL_INCLUDE_BSL_BITSET
# ifndef INCLUDED_BSLSTL_BITSET
#   include <bslstl_bitset.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_DEQUE
# ifndef INCLUDED_BSLSTL_DEQUE
#   include <bslstl_deque.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_VECTOR
# ifndef INCLUDED_BSLSTL_VECTOR
#   include <bslstl_vector.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_MAP
# ifndef INCLUDED_BSLSTL_MAP
#   include <bslstl_map.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_MULTIMAP
#   include <bslstl_multimap.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_SET
# ifndef INCLUDED_BSLSTL_SET
#   include <bslstl_set.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_MULTISET
#   include <bslstl_multiset.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_UNORDERED_MAP
# ifndef INCLUDED_BSLSTL_UNORDEREDMAP
#   include <bslstl_unorderedmap.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_UNORDEREDMULTIMAP
#   include <bslstl_unorderedmultimap.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_UNORDERED_SET
# ifndef INCLUDED_BSLSTL_UNORDEREDSET
#   include <bslstl_unorderedset.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_UNORDEREDMULTISET
#   include <bslstl_unorderedmultiset.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

// The following include provides backwards compatibility for Bloomberg-managed
// code that requires 'stdexcept' to include 'string'.  It is placed here
// because placing it directly in 'bsl_stdexcept.h' breaks the delicate include
// graph of the native headers.
#if defined(INCLUDED_NATIVE_STDEXCEPT) && defined(BSL_INCLUDE_NATIVE_STRING)
# ifndef INCLUDED_NATIVE_STRING
#   include <string>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_ITERATOR
# ifndef INCLUDED_BSLSTL_ITERATOR
#   include <bslstl_iterator.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif

// Also include iterator adapters.

# ifndef INCLUDED_BSLSTL_FORWARDITERATOR
#   include <bslstl_forwarditerator.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif

# ifndef INCLUDED_BSLSTL_BIDIRECTIONALITERATOR
#   include <bslstl_bidirectionaliterator.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
#  endif

# ifndef INCLUDED_BSLSTL_RANDOMACCESSITERATOR
#   include <bslstl_randomaccessiterator.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_STRING
# ifndef INCLUDED_BSLSTL_STRING
#   include <bslstl_string.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_STRINGREF
#   include <bslstl_stringref.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

// If '<utility>' was included, then include our implementation of 'pair'.
#ifdef INCLUDED_NATIVE_UTILITY
# ifndef INCLUDED_BSLSTL_PAIR
#   include <bslstl_pair.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

// If '<functional>' was included, then include SGI extension to functional.
#ifdef INCLUDED_NATIVE_FUNCTIONAL
# ifndef INCLUDED_BSLSTP_EXFUNCTIONAL
#   include <bslstp_exfunctional.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_HASH
#   include <bslstl_equalto.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif
# ifndef INCLUDED_BSLSTL_HASH
#   include <bslstl_hash.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

// If '<memory>' was included, then include our implementation of allocators.
#ifdef INCLUDED_NATIVE_MEMORY
# ifndef INCLUDED_BSLSTL_ALLOCATOR
#   include <bslstl_allocator.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

// Recursively re-try this header if there are any native STL headers for which
// corresponding 'bslstp' or 'bslstl' headers still need to be included.
#if defined(INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE)
# undef INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# include <bsl_stdhdrs_epilogue_recursive.h>
#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
