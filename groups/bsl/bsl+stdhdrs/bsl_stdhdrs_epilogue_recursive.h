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
# ifndef INCLUDED_BSLSTL_ALLOCATOR
#   include <bslstl_allocator.h>
#   define INCLUDE_BSL_STDHDRS_EPILOGUE_RECURSIVE
# endif
# ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#   include <bslstl_allocatortraits.h>
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
