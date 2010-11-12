// bsls_logicerror.h                  -*-C++-*-
#ifndef INCLUDED_BSLS_LOGICERROR
#define INCLUDED_BSLS_LOGICERROR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a low-level "logic error" exception.
//
//@DEPRECATED: Use 'std::logic_error' instead.
//
//@CLASSES:
//  bsls_LogicError: exception class for a low-level logic error
//
//@AUTHOR: Pablo Halpern (phalpern), John Lakos (jlakos)
//

#ifndef INCLUDED_STDEXCEPT
#include <stdexcept>
#endif

namespace BloombergLP {

// =====================
// class bsls_LogicError
// =====================

typedef std::logic_error bsls_LogicError;

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
