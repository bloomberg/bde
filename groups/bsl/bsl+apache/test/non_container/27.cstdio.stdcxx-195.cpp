/************************************************************************
 *
 * 27.cstdio.stdcxx-195.cpp - regression test for STDCXX-195
 *
 * $Id: 27.cstdio.stdcxx-195.cpp 609466 2008-01-06 23:18:56Z sebor $
 *
 ***************************************************************************
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

#include <cstdio>


#ifdef _RWSTD_STRICT_ANSI

#  ifdef fpos_t
#    error "fpos_t #defined"
#  endif

#  ifdef remove
#    error "remove #defined"
#  endif

#  ifdef rename
#    error "rename #defined"
#  endif

#  ifdef tmpfile
#    error "tmpfile #defined"
#  endif

#  ifdef tmpnam
#    error "tmpnam #defined"
#  endif

#  ifdef fclose
#    error "fclose #defined"
#  endif

#  ifdef fflush
#    error "fflush #defined"
#  endif

#  ifdef fopen
#    error "fopen #defined"
#  endif

#  ifdef freopen
#    error "freopen #defined"
#  endif

#  ifdef setbuf
#    error "setbuf #defined"
#  endif

#  ifdef setvbuf
#    error "setvbuf #defined"
#  endif

#  ifdef fprintf
#    error "fprintf #defined"
#  endif

#  ifdef fscanf
#    error "fscanf #defined"
#  endif

#  ifdef printf
#    error "printf #defined"
#  endif

#  ifdef scanf
#    error "scanf #defined"
#  endif

#  ifdef sprintf
#    error "sprintf #defined"
#  endif

#  ifdef sscanf
#    error "sscanf #defined"
#  endif

#  ifdef vfprintf
#    error "vfprintf #defined"
#  endif

#  ifdef vprintf
#    error "vprintf #defined"
#  endif

#  ifdef vsprintf
#    error "vsprintf #defined"
#  endif

#  ifdef fgetc
#    error "fgetc #defined"
#  endif

#  ifdef fgets
#    error "fgets #defined"
#  endif

#  ifdef fputc
#    error "fputc #defined"
#  endif

#  ifdef fputs
#    error "fputs #defined"
#  endif

#  ifdef getc
#    error "getc #defined"
#  endif

#  ifdef getchar
#    error "getchar #defined"
#  endif

#  ifdef gets
#    error "gets #defined"
#  endif

#  ifdef putc
#    error "putc #defined"
#  endif

#  ifdef putchar
#    error "putchar #defined"
#  endif

#  ifdef puts
#    error "puts #defined"
#  endif

#  ifdef ungetc
#    error "ungetc #defined"
#  endif

#  ifdef fread
#    error "fread #defined"
#  endif

#  ifdef fwrite
#    error "fwrite #defined"
#  endif

#  ifdef fgetpos
#    error "fgetpos #defined"
#  endif

#  ifdef fseek
#    error "fseek #defined"
#  endif

#  ifdef fsetpos
#    error "fsetpos #defined"
#  endif

#  ifdef ftell
#    error "ftell #defined"
#  endif

#  ifdef rewind
#    error "rewind #defined"
#  endif

#  ifdef clearerr
#    error "clearerr #defined"
#  endif

#  ifdef feof
#    error "feof #defined"
#  endif

#  ifdef ferror
#    error "ferror #defined"
#  endif

#  ifdef perror
#    error "perror #defined"
#  endif

#endif   // _RWSTD_STRICT_ANSI

int main ()
{
    return 0;
}
