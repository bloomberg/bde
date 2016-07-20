// bsl_stdhdrs_epilogue_recursive.h                                   -*-C++-*-

//@PURPOSE: Provide corresponding bslstl definitions for C++ standard headers.
//
//@SEE_ALSO: bsl_stdhdrs_epilogue, bsl+stdhdrs.txt
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


// Now include those 'bslstl' components corresponding to the 'bsl+stdhdrs'
// files that have been included.

#ifdef BSL_INCLUDE_BSL_SSTREAM
# ifndef INCLUDED_BSLSTL_ISTRINGSTREAM
#   include <bslstl_istringstream.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif

# ifndef INCLUDED_BSLSTL_OSTRINGSTREAM
#   include <bslstl_ostringstream.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif

# ifndef INCLUDED_BSLSTL_STRINGBUF
#   include <bslstl_stringbuf.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif

# ifndef INCLUDED_BSLSTL_STRINGSTREAM
#   include <bslstl_stringstream.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

#ifdef BSL_INCLUDE_BSL_IOSFWD
# ifndef INCLUDED_BSLSTL_IOSFWD
#   include <bslstl_iosfwd.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

// If '<algorithm>' was included, then inclue the work-arounds to the platform
// algorithms header.
#ifdef INCLUDED_NATIVE_ALGORITHM
# ifndef INCLUDED_BSLSTL_ALGORITHMWORKAROUND
#   include <bslstl_algorithmworkaround.h>
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

#ifdef BSL_INCLUDE_BSL_LIST
# ifndef INCLUDED_BSLSTL_LIST
#   include <bslstl_list.h>
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

#ifdef BSL_INCLUDE_BSL_QUEUE
# ifndef INCLUDED_BSLSTL_QUEUE
#   include <bslstl_queue.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_PRIORITYQUEUE
#   include <bslstl_priorityqueue.h>
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

#ifdef BSL_INCLUDE_BSL_STACK
# ifndef INCLUDED_BSLSTL_STACK
#   include <bslstl_stack.h>
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

// If '<functional>' was included, then include extensions to functional.
#ifdef INCLUDED_NATIVE_FUNCTIONAL
# ifndef INCLUDED_BSLSTL_EQUALTO
#   include <bslstl_equalto.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_FUNCTION
#   include <bslstl_function.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_HASH
#   include <bslstl_hash.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_REFERENCEWRAPPER
#   include <bslstl_referencewrapper.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

// If '<memory>' was included, then include our implementation of allocators,
// 'allocator_traits', and 'shared_ptr'
#ifdef INCLUDED_NATIVE_MEMORY
# ifndef INCLUDED_BSLMA_STDALLOCATOR
#   include <bslma_stdallocator.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_BADWEAKPTR
#   include <bslstl_badweakptr.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_OWNERLESS
#   include <bslstl_ownerless.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_SHAREDPTR
#   include <bslstl_sharedptr.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_SHAREDPTRALLOCATEINPLACEREP
#   include <bslstl_sharedptrallocateinplacerep.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_SHAREDPTRALLOCATEOUTOFPLACEREP
#   include <bslstl_sharedptrallocateoutofplacerep.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLMA_ALLOCATORTRAITS
#   include <bslma_allocatortraits.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
#endif

// If '<type_traits>' was included, then include our implementation of many
// type traits.
#ifdef INCLUDED_NATIVE_TYPE_TRAITS
// level 1
# ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#   include <bslmf_integralconstant.h>    // This must be included first
# endif
// level 2
# ifndef INCLUDED_BSLMF_ISARRAY
#   include <bslmf_isarray.h>
# endif
# ifndef INCLUDED_BSLMF_ISCONST
#   include <bslmf_isconst.h>
# endif
# ifndef INCLUDED_BSLMF_ISLVALUEREFERENCE
#   include <bslmf_islvaluereference.h>
# endif
# ifndef INCLUDED_BSLMF_ISRVALUEREFERENCE
#   include <bslmf_isrvaluereference.h>
# endif
# ifndef INCLUDED_BSLMF_ISVOLATILE
#   include <bslmf_isvolatile.h>
# endif
// level 3
# ifndef INCLUDED_BSLMF_ISREFERENCE
#   include <bslmf_isreference.h>
# endif
// level 4
# ifndef INCLUDED_BSLMF_ISSAME
#   include <bslmf_issame.h>
# endif
// level 5
# ifndef INCLUDED_BSLMF_ADDLVALUEREFERENCE
#   include <bslmf_addlvaluereference.h>
# endif
# ifndef INCLUDED_BSLMF_ADDRVALUEREFERENCE
#   include <bslmf_addrvaluereference.h>
# endif
# ifndef INCLUDED_BSLMF_CONDITIONAL
#   include <bslmf_conditional.h>
# endif
# ifndef INCLUDED_BSLMF_REMOVECONST
#   include <bslmf_removeconst.h>
# endif
# ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#   include <bslmf_removereference.h>
# endif
# ifndef INCLUDED_BSLMF_REMOVEVOLATILE
#   include <bslmf_removevolatile.h>
# endif
// level 6
# ifndef INCLUDED_BSLMF_ADDPOINTER
#   include <bslmf_addpointer.h>
# endif
# ifndef INCLUDED_BSLMF_ADDREFERENCE
#   include <bslmf_addreference.h>
# endif
# ifndef INCLUDED_BSLMF_REMOVECV
#   include <bslmf_removecv.h>
# endif
// level 7
# ifndef INCLUDED_BSLMF_ISCLASS
#   include <bslmf_isclass.h>
# endif
# ifndef INCLUDED_BSLMF_ISFLOATINGPOINT
#   include <bslmf_isfloatingpoint.h>
# endif
# ifndef INCLUDED_BSLMF_ISFUNCTION
#   include <bslmf_isfunction.h>
# endif
# ifndef INCLUDED_BSLMF_ISINTEGRAL
#   include <bslmf_isintegral.h>
# endif
# ifndef INCLUDED_BSLMF_ISMEMBERFUNCTIONPOINTER
#   include <bslmf_ismemberfunctionpointer.h>
# endif
# ifndef INCLUDED_BSLMF_ISPOINTER
#   include <bslmf_ispointer.h>
# endif
# ifndef INCLUDED_BSLMF_ISVOID
#   include <bslmf_isvoid.h>
# endif
// level 8
# ifndef INCLUDED_BSLMF_ADDCONST
#   include <bslmf_addconst.h>
# endif
# ifndef INCLUDED_BSLMF_ADDVOLATILE
#   include <bslmf_addvolatile.h>
# endif
# ifndef INCLUDED_BSLMF_ISARITHMETIC
#   include <bslmf_isarithmetic.h>
# endif
# ifndef INCLUDED_BSLMF_ISEMPTY
#   include <bslmf_isempty.h>
# endif
# ifndef INCLUDED_BSLMF_ISMEMBEROBJECTPOINTER
#   include <bslmf_ismemberobjectpointer.h>
# endif
# ifndef INCLUDED_BSLMF_ISPOLYMORPHIC
#   include <bslmf_ispolymorphic.h>
# endif
# ifndef INCLUDED_BSLMF_REMOVEPOINTER
#   include <bslmf_removepointer.h>
# endif
// level 9
# ifndef INCLUDED_BSLMF_ADDCV
#   include <bslmf_addcv.h>
# endif
# ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#   include <bslmf_isfundamental.h>
# endif
# ifndef INCLUDED_BSLMF_ISMEMBERPOINTER
#   include <bslmf_ismemberpointer.h>
# endif
// level 10
# ifndef INCLUDED_BSLMF_ENABLEIF
#   include <bslmf_enableif.h>
# endif
// level 11
# ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#   include <bslmf_isconvertible.h>
# endif
// level 12
// level 13
# ifndef INCLUDED_BSLMF_ISENUM
#   include <bslmf_isenum.h>
# endif
// level 14
# ifndef INCLUDED_BSLMF_ISCOPYCONSTRUCTIBLE
#   include <bslmf_iscopyconstructible.h>
# endif
# ifndef INCLUDED_BSLMF_ISNOTHROWMOVECONSTRUCTIBLE
#   include <bslmf_isnothrowmoveconstructible.h>
# endif
# ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#   include <bslmf_istriviallycopyable.h>
# endif
# ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#   include <bslmf_istriviallydefaultconstructible.h>
# endif
// level 15

#endif


// Recursively re-try this header if there are any native STL headers for which
// corresponding 'bslstp' or 'bslstl' headers still need to be included.
#if defined(INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE)
# undef INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# include <bsl_stdhdrs_epilogue_recursive.h>
#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
