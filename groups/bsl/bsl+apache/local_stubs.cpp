/* local_stubs.h                                                     -*-C-*- */

// Local stubs for rw functions.  Not part of Apache STL distrubution.

#ifndef INCLUDED_LOCAL_STUBS_DOT_H
#define INCLUDED_LOCAL_STUBS_DOT_H

//#include <rw_printf.h>
//#include <testdefs.h>
#include <rw/_defs.h>
#include <exception>
#include <cstdio>
#include <cstdlib>

//#include <cstdarg>

_RWSTD_NAMESPACE(_RW) {

void __rw_assert_fail(const char* expr, const char* file, int line,
               const char* func)
{
    std::fprintf(stderr,
                 "%s:%d: error: Assertion failed, %s, in function %s\n",
                 file, line, expr, func);
    std::abort();
}

_RWSTD_SSIZE_T
__rw_memattr (const void*, size_t, int)
{
    return 0;
}

static void default_throw_proc(int i, char* s)
{
    throw std::exception();
}

void (*__rw_throw_proc)(int, char*) = default_throw_proc;

}

#endif // ! defined(INCLUDED_LOCAL_STUBS_DOT_H)
