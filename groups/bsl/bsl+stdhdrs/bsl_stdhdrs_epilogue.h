// bsl_stdhdrs_epilogue.h                                             -*-C++-*-

//@PURPOSE: Provide initial declarations for each C++ standard header file.
//
//@SEE_ALSO: bsl_stdhdrs_prologue, bsl_stdhdrs_epilogue_recursive
//
//@AUTHOR: Pablo Halpern (phalpern), Arthur Chiu (achiu21)
//
//@DESCRIPTION: This epilogue file is intended to be included at the end of
// each BSL-supplied C++ standard library header file in "BDE-STL" mode.  This
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

#ifndef INCLUDED_BSLS_CODEBASE
#include <bsls_codebase.h>
#endif

#ifndef BSL_OVERRIDES_STD
#error "Epilogue should not be included in native STL mode"
#endif

// Include corresponding 'bslstp' and 'bslstl' headers for each native STL
// header.  'bsl_stdhdrs_epilogue_recursive.h' is the implementation of the
// epilogue that gets included recursively until all native STL headers are
// matched with their corresponding 'bslstp' and 'bslstl' headers.
#define USE_BSL_STDHDRS_EPILOGUE_RECURSIVE
#include <bsl_stdhdrs_epilogue_recursive.h>
#undef USE_BSL_STDHDRS_EPILOGUE_RECURSIVE

// Defining the 'std' macro allows Bloomberg-managed code (Robo) to use 'std::'
// instead of 'bsl::' for Bloomberg's implementation of the STL containers.
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
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
