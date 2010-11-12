// bsl_stdhdrs_epilogue.h                                             -*-C++-*-

//@PURPOSE: Provide initial declarations for each C++ standard header file.
//
//@SEE_ALSO: bsl_stdhdrs_prologue.h
//
//@AUTHOR: Pablo Halpern (phalpern), Arthur Chiu (achiu21)
//
//@DESCRIPTION: This epilogue file is intended to be included at the end of
// each BSL-supplied C++ standard library header file.  In bde-stl mode, the
// header '#include's bsl versions of some standard components, overriding
// those symbols included from the native standard library.  This epilogue also
// defines 'std' as a macro that expands to 'bsl', which contains a blend of
// symbols from the native standard library and Bloomberg defined container
// types.  The inclusion of this file at the end of a 'bsl+stdhdrs' header must
// be paired with the inclusion of 'bsl_stdhdrs_prologue.h' at the start of the
// same header.
//
// Note that there are no include guards.  THIS FILE IS INTENDED FOR MULTIPLE
// INCLUSION.

#ifndef BSL_OVERRIDES_STD
#error "Epilogue shouldn't be included in native stl mode"
#endif

// Include those 'bsl' components corresponding to the 'bsl+stdhdrs' files that
// have been included.  Because a native standard header may include another
// standard header, and because we intercept all standard headers, there is a
// danger that we could introduce our own headers (with different dependencies)
// into the include graph of the native headers, accidentally introducing a
// cycle.  To avoid this problem, we defer inclusion of our own headers until
// this epilogue, when the top-most native header has been fully processed.
// See 'bsl+stdhdrs.txt' for a more complete explanation.

// Headers need to be in roughly reverse-dependency order, where the components
// with most dependencies are included first, followed by those that they
// depend on.  Thus if bslstl_X includes <Y>, and Y uses bslstl_Y, then
// bslstl_Y will be included after bslstl_X in this epilogue.  If the includes
// were in the opposite order, then bslstl_Y would not be included until a
// (possible) future inclusion of this epilogue.  See 'bsl+stdhdrs.txt' for a
// more complete explanation.

// First start with STLPort containers, which may include native headers.  The
// 'BSL_INCLUDE_*' macros are defined in the standard headers inside the
// 'bsl+stdhdrs' package.

#ifdef BSL_INCLUDE_BSL_LIST
# ifndef INCLUDED_BSLSTP_LIST
#   include <bslstp_list.h>
# endif
#endif

#ifdef BSL_INCLUDE_BSL_MAP
# ifndef INCLUDED_BSLSTP_MAP
#   include <bslstp_map.h>
# endif
#endif

#ifdef BSL_INCLUDE_BSL_QUEUE
# ifndef INCLUDED_BSLSTP_QUEUE
#   include <bslstp_queue.h>
# endif
#endif

#ifdef BSL_INCLUDE_BSL_SET
# ifndef INCLUDED_BSLSTP_SET
#   include <bslstp_set.h>
# endif
#endif

#ifdef BSL_INCLUDE_BSL_STACK
# ifndef INCLUDED_BSLSTP_STACK
#   include <bslstp_stack.h>
# endif
#endif

#ifdef BSL_INCLUDE_BSL_HASH_MAP
# ifndef INCLUDED_BSLSTP_HASHMAP
#   include <bslstp_hashmap.h>
# endif
#endif

#ifdef BSL_INCLUDE_BSL_HASH_SET
# ifndef INCLUDED_BSLSTP_HASHSET
#   include <bslstp_hashset.h>
# endif
#endif

#ifdef BSL_INCLUDE_BSL_SLIST
# ifndef INCLUDED_BSLSTP_SLIST
#   include <bslstp_slist.h>
# endif
#endif

#ifdef BSL_INCLUDE_BSL_SSTREAM
# ifndef INCLUDED_BSLSTP_SSTREAM
#   include <bslstp_sstream.h>
# endif
#endif

// Special case: If <algorithm> was included, then include sgi extension to
// algorithms.  <algorithm> uses pair and iterators, so we place it before the
// two component headers here in epilogue.  Note that Bloomberg does not
// implement the basic algorithms (i.e., there is no 'bslstl_algorithm').
#ifdef INCLUDED_NATIVE_ALGORITHM
# ifndef INCLUDED_BSLSTL_EXALGORITHM
#   include <bslstl_exalgorithm.h>
# endif
#endif

// The bslstl containers are fairly clean, and don't cause many dependency
// problems.   Even though they include <iterator>, they do so indirectly
// through bslstl_iterator.  They do include <memory> for allocator though, so
// they must be included before 'bslstl_allocator'.
#ifdef BSL_INCLUDE_BSL_BITSET
# ifndef INCLUDED_BSLSTL_BITSET
#   include <bslstl_bitset.h>
# endif
#endif

#ifdef BSL_INCLUDE_BSL_DEQUE
# ifndef INCLUDED_BSLSTL_DEQUE
#   include <bslstl_deque.h>
# endif
#endif

#ifdef BSL_INCLUDE_BSL_VECTOR
# ifndef INCLUDED_BSLSTL_VECTOR
#   include <bslstl_vector.h>
# endif
#endif

// This include provides backwards compatibility for Bloomberg managed code
// that requires 'stdexcept' to include 'string'.  It is placed here because
// placing it directly in 'bsl_stdexcept.h' breaks the delicate include graph
// of the native headers.
#if defined(INCLUDED_NATIVE_STDEXCEPT) && defined(BSL_INCLUDE_NATIVE_STRING)
# ifndef INCLUDED_NATIVE_STRING
#   include <string>
# endif
#endif

// <bslstl_iterator.h> includes <iterator>, which includes <string> on some
// platforms, so <bslstl_iterator.h> must be included before <bslstl_string.h>.
#ifdef BSL_INCLUDE_BSL_ITERATOR
# ifndef INCLUDED_BSLSTL_ITERATOR
#   include <bslstl_iterator.h>
# endif
#endif

#ifdef BSL_INCLUDE_BSL_STRING
# ifndef INCLUDED_BSLSTL_STRING
#   include <bslstl_string.h>
# endif
#endif

// <bslstl_iterator.h> includes <iterator>, which includes <locale> on some
// platforms, which then includes <iosfwd>, so <bslstp_iosfwd.h> must be
// included after <bslstl_iterator.h>.
#ifdef BSL_INCLUDE_BSL_IOSFWD
# ifndef INCLUDED_BSLSTP_IOSFWD
#   include <bslstp_iosfwd.h>
# endif
#endif

// Very special case: To maintain transitive includes inside Bloomberg managed
// libraries (robo), <string> will include <algorithm>, which causes a
// cycle, as algorithm then includes <iterator>, which sometimes includes
// <string>.  As <algorithm> should pull in <bslstl_exalgorithm.h>, we need to
// place the include of <bslstl_exalgorithm.h> after <string>.  If
// <algorithm> is included directly, then the duplicated include for
// <algorithm> will properly include <string>.
#ifdef INCLUDED_NATIVE_ALGORITHM
# ifndef INCLUDED_BSLSTL_EXALGORITHM
#   include <bslstl_exalgorithm.h>
# endif
#endif

// Lowest level: memory and utility components.

// Special case: If <utility> was included, then include our implementation of
// pair<T1,T2>.
#ifdef INCLUDED_NATIVE_UTILITY
# ifndef INCLUDED_BSLSTL_PAIR
#   include <bslstl_pair.h>
# endif
#endif

// Special case: If <functional> was included, then include sgi extension to
// functional.
#ifdef INCLUDED_NATIVE_FUNCTIONAL
# ifndef INCLUDED_BSLSTL_EXFUNCTIONAL
#   include <bslstl_exfunctional.h>
# endif
#endif

// Special case: If <memory> was included, then include our implementation of
// allocators.
#ifdef INCLUDED_NATIVE_MEMORY
# ifndef INCLUDED_BSLSTL_ALLOCATOR
#   include <bslstl_allocator.h>
# endif
#endif

// This macro allows Bloomberg managed code (robo) to use 'std::' instead of
// 'bsl::' for Bloomberg's implementation of the STL containers.
#ifdef std
# error "A macro for 'std' should not be '#define'd outside of 'bsl+stdhdrs'."
#elif defined(BSL_OVERRIDES_STD)
# define std bsl
# define BSL_STDHDRS_DEFINED_STD_MACRO
#endif

#undef BSL_STDHDRS_PROLOGUE_IN_EFFECT
#undef BSL_STDHDRS_RUN_EPILOGUE

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
