/************************************************************************
 *
 * rw_braceexp.h - declarations of testsuite helpers
 *
 * $Id: rw_braceexp.h 648752 2008-04-16 17:01:56Z faridz $
 *
 ************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 **************************************************************************/

#ifndef RW_BRACEEXP_H_INCLUDED
#define RW_BRACEEXP_H_INCLUDED


#include <testdefs.h>

// rw_brace_expand() performs brace expansion similar to the csh shell.
// the base grammar for the brace expansion is supposed to be..
//
//   string     ::= <brace-expr> | [ <chars> ]
//   brace-expr ::= <string> '{' <brace-list> | <brace-sequ> '}' <string> |
//                  <string>
//   brace-list ::= <string> ',' <brace-list> | <string>
//   brace-sequ ::= <upper>   '.' '.' <upper>   |
//                  <lower>   '.' '.' <lower>   |
//                  <integer> '.' '.' <integer>
//   chars      ::= <pcs-char> <string> | <pcs-char>
//   integer    ::= ['+' | '-'] <digits>
//   upper      ::= pcs-char 'a-z'
//   lower      ::= pcs-char 'A-Z'
//   digit      ::= pcs-char '0-9'
//   digits     ::= <digit> <digits> | <digit>
//   pcs-char   ::= character in the Portable Character Set
//
// many examples can be found in the test 0.braceexp.cpp.
//

//
// this function will attempt to expand `sz' bytes of the brace expression
// `brace_expr'  into `n' bytes of the  output buffer `s', seperating each
// expansion with a single seperator character `sep'. if the output buffer
// `s' is is null, or the  number of bytes `n' is  insufficient to contain
// all  expansions of `brace_expr', an  appropriately sized buffer will be
// allocated  with malloc(). a  pointer to the  output buffer that is used
// will be  returned. if the  pointer  returned  is not  equal to the user
// supplied  input  buffer `s', then the caller is  expected to free() the
// returned pointer.
//
// this  function can  return null  if  the  brace  expansion could not be
// processed. this can happen if, for example, the brace expression string
// contains an unmatched unescaped open brace.  the function can also fail
// and return null if a memory allocation request fails.
//
_TEST_EXPORT char*
rw_brace_expand (const char* brace_expr, _RWSTD_SIZE_T sz,
                 char* s, _RWSTD_SIZE_T n, char sep);


//
// this  function is  similar to  rw_brace_expand, except  that the  input
// string `shell_expr' is tokenized on whitespace, and each non-whitespace
// token is expanded seperately. this function will fail if an attempt to
// brace expand one of the tokens fails, regardless of the reason for that
// failure.
//
// the caller may need to free() the returned pointer. please see comments
// above for details.
//
// this  function only does tokenization and brace expansion at this time.
// at some point it may make  sense to add environment variable expansion.
//
_TEST_EXPORT char*
rw_shell_expand (const char* shell_expr, _RWSTD_SIZE_T sz,
                 char* s, _RWSTD_SIZE_T n, char sep);


#endif   // RW_BRACEEXP_H_INCLUDED

