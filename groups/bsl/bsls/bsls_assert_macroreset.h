// bsls_assert_macroreset.h                                           -*-C++-*-
#ifndef INCLUDED_BSLS_ASSERT_MACRORESET
#define INCLUDED_BSLS_ASSERT_MACRORESET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Undefine all macros from 'bsls_assert.h' and 'bsls_review.h'.
//
//@DESCRIPTION: This component provides a header that is *included* to undefine
// all macros defined in 'bsls_assert.h' and 'bsls_review.h'; it is designed to
// be *re*-includable.  This is for the use of the 'bsls_assert' test driver to
// enable it to re-include the 'bsls_assert.h' header with different input
// macros and verify that they output the correct resulting macro definitions.
//
///Usage
///-----
// To undefine all macros defined in 'bsls_assert.h' and 'bsls_review.h', first
// disable the header include guard for this header
// ('bsls_assert_macroreset.h'):
//..
//  #undef INCLUDED_BSLS_ASSERT_MACRORESET
//..
// Then re-include this header, which will undefine the macros that were
// defined by 'bsls_assert.h' and 'bsls_review.h':
//..
//  #include <bsls_assert_macroreset.h>
//..
// Then define any macros that control the behaviors of 'bsls_assert.h' and
// 'bsls_review.h' for this particular test, then re-include 'bsls_assert.h':
//..
//  #include <bsls_assert.h>
//..

// Undefine the external header guards so that the component headers can be
// re-included and re-evaluated.
#undef INCLUDED_BSLS_REVIEW
#undef INCLUDED_BSLS_ASSERT

// Undefine the build mode so it can be set to whatever the test wants to
// verify next.
#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

// Undefine the explicit review level and assert level macros.
#undef BSLS_REVIEW_LEVEL_REVIEW
#undef BSLS_REVIEW_LEVEL_REVIEW_OPT
#undef BSLS_REVIEW_LEVEL_REVIEW_SAFE
#undef BSLS_REVIEW_LEVEL_NONE

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

// Undefine the macros actually defined by 'bsls_review.h'.
#undef BSLS_REVIEW
#undef BSLS_REVIEW_REVIEW_IMP
#undef BSLS_REVIEW_DISABLED_IMP
#undef BSLS_REVIEW_INVOKE
#undef BSLS_REVIEW_IS_ACTIVE
#undef BSLS_REVIEW_OPT
#undef BSLS_REVIEW_OPT_IS_ACTIVE
#undef BSLS_REVIEW_SAFE
#undef BSLS_REVIEW_SAFE_IS_ACTIVE

// Undefine the macros actually defined by 'bsls_assert.h'.
#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_DISABLED_IMP
#undef BSLS_ASSERT_INVOKE
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
