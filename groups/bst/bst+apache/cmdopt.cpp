/***************************************************************************
 *
 * $Id: cmdopt.cpp 650698 2008-04-22 23:38:40Z sebor $
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
 * Copyright 2005-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <cmdopt.h>

#include <ctype.h>    // isdigit(), isspace()
#include <errno.h>    // for errno
#include <stdarg.h>   // for va_arg, ...
#include <stdio.h>    // for fprintf
#include <stdlib.h>   // for atexit, free, malloc
#include <string.h>   // for memcpy, strcpy, strcmp, ...

#ifndef EINVAL
#  define EINVAL   22   /* e.g., HP-UX, Linux, Solaris */
#endif   // EINVAL

/**************************************************************************/

typedef int (optcallback_t)(int, char*[]);

struct cmdopts_t
{
    char           loptbuf_ [32];   // buffer for long option name
    optcallback_t *callback_;       // function to call to process option

    // counter to increment for each occurrence of an option
    // or to set to the numeric argument (when specified)
    int           *pcntr_;

    char          *lopt_;           // long option name
    char           sopt_;           // short option name

    int            minval_;         // minimum value of a numerical argument
    int            maxval_;         // maximum value of a numerical argument
    size_t         maxcount_;       // how many times option can be invoked
    size_t         count_;          // how many times it has been invoked

    unsigned       arg_ : 1;        // option takes an argument
    unsigned       inv_ : 1;        // callback invocation inverted
    unsigned       tristate_ : 1;   // option is a tristate
    unsigned       envseen_ : 1;    // environment option already processed
};


// total number of registered options
static size_t _rw_ncmdopts;

// number of default (always defined) options
static size_t     _rw_ndefopts;
static cmdopts_t  _rw_cmdoptbuf [32];
static cmdopts_t *_rw_cmdopts = _rw_cmdoptbuf;
static size_t     _rw_optbufsize = sizeof _rw_cmdoptbuf / sizeof *_rw_cmdoptbuf;

/**************************************************************************/

static int
_rw_print_help (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Without an argument, prints this help to stdout and exits with\n"
            "a status of 0 without further processing.\n"
            "With the optional argument prints help on the option with that\n"
            "name, if one exists, and exits with a status of 0. If an option\n"
            "with the specified name does not exist, prints an error message\n"
            "to stderr and exits with a status of 1.\n"
            "The leading underscores in the name of an option are optional.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    // the option name to get help on, if any
    const char* opthelp = 1 < argc ? argv [1] : 0;

    // remove the optional one or two leading underscores
    if (opthelp && '-' == opthelp [0] && opthelp [1])
        opthelp += 1 + ('-' == opthelp [1]);

    if (0 == opthelp)
        printf ("OPTIONS\n");

    // set to a non-zero when the specified option is found
    int option_found = 0;

    for (size_t i = 0; i != _rw_ncmdopts; ++i) {

        // for convenience
        const cmdopts_t* const opt = _rw_cmdopts + i;

        // get a pointer to the name of the long option, if any
        const char* const lopt = opt->lopt_ ? opt->lopt_ : opt->loptbuf_;

        if (opthelp && *opthelp) {

            if (   opt->sopt_ == opthelp [0] && '\0' == opthelp [1]
                || *lopt && 0 == strcmp (lopt + 1, opthelp)) {

                // remember that we found the option whose (short
                // or long) name we're to give help on; after printing
                // the help text on the option keep looping in case
                // there is another option and callback with the same
                // name (unlikely but possible)
                option_found = 1;
            }
            else {
                // the option doesn't match, continue searching
                continue;
            }
        }

        // separate options with help functionality (which typically
        // produce multiline output) from previous options without
        // it (and thus very brief output)
        if (i && 0 == _rw_cmdopts [i - 1].callback_ && opt->callback_)
            puts ("");

        printf ("     ");

        if (opt->sopt_) {
            printf ("-%c", opt->sopt_);

            if (lopt)
                printf (" | ");
        }

        const char *pfx = "";
        const char *sfx = pfx;

        if (lopt) {
            printf ("-%s", lopt);
            if (opt->arg_ && '=' != lopt [strlen (lopt) - 1]) {
                pfx = " [ ";
                sfx = " ]";
            }
        }

        const char* const arg =
               _RWSTD_INT_MIN < opt->minval_
            || opt->maxval_ < _RWSTD_INT_MAX ? "int" : "arg";

        if (opt->arg_) {
            // print argument (in brackets when it's optional)
            printf ("%s<%s>%s", pfx, arg, sfx);
        }

        if (opt->pcntr_)
            printf (" | -%s=<%s>", lopt, arg);

        if ('i' == *arg) {
            printf (", with ");

            if (_RWSTD_INT_MIN < opt->minval_)
                printf ("%d <= ", opt->minval_);

            printf ("<%s>", arg);

            if (opt->maxval_ < _RWSTD_INT_MAX)
                printf (" <= %d", opt->maxval_);

            printf ("\n      ");
        }

        if (_RWSTD_SIZE_MAX == opt->maxcount_)
            printf (" (each occurrence evaluated)\n");
        else if (1 < opt->maxcount_)
            printf (" (at most %u occurrences evaluated)\n",
                    unsigned (opt->maxcount_));
        else
            printf (" (only the first occurrence evaluated)\n");

        // invoke callback with the "--help" option
        if (opt->callback_) {

            char* help [2] = { 0, 0 };

            opt->callback_ (1, help);

            for (const char *line = help [0]; line; ) {

                const char* const nl = strchr (line, '\n');
                const int len = nl ? int (nl - line) : int (strlen (line));

                printf ("       %.*s\n", len, line);

                line = nl;
                if (nl)
                    ++line;
            }
        }
    }

    if (opthelp && !option_found) {
        fprintf (stderr, "Unknown option \"%s\".\n", opthelp);
        exit (1);
    }

    exit (0);

    return 0;
}

/**************************************************************************/

static int
_rw_set_ignenv (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Prevents options specified in the RWSTD_TESTOPTS environment\n"
            "variable from taking effect.\n"
            "Unless this option is specified, the RWSTD_TESTOPTS environment\n"
            "variable is processed as if its value were specified on the \n"
            "command line.\n"
            "For example, setting the value of the variable to the string\n"
            "\"--verbose --no-wchar\" and invoking this program with no\n"
            "command line arguments will have the same effect as invoking\n"
            "it with the two arguments on the command line.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    return 0;
}

extern "C" {

static void
_rw_clear_opts ()
{
    // reset all options, deallocating dynamically allocated storage

    for (size_t i = 0; i != _rw_ncmdopts; ++i) {

        // free any storage allocated for the option name
        free (_rw_cmdopts [i].lopt_);
    }

    if (_rw_cmdopts != _rw_cmdoptbuf) {
        // free the storage allocated for all the options
        free (_rw_cmdopts);
    }

    // reset the options pointer to point at the statically
    // allocated buffer and the count back to 0
    _rw_ncmdopts   = 0;
    _rw_cmdopts    = _rw_cmdoptbuf;
    _rw_optbufsize = sizeof _rw_cmdoptbuf / sizeof *_rw_cmdoptbuf;
}

}

static void
_rw_set_myopts ()
{
    static int cleanup_handler_registered;

    if (0 == cleanup_handler_registered) {
        atexit (_rw_clear_opts);
        cleanup_handler_registered = 1;
    }

    if (0 != _rw_ncmdopts)
        return;

    static int recursive;

    if (recursive)
        return;

    ++recursive;

    rw_setopts (""
                "|-help: "       // argument optional
                "|-ignenv ",
                _rw_print_help,
                _rw_set_ignenv);

    _rw_ndefopts = _rw_ncmdopts;

    recursive = 0;
}

/**************************************************************************/

#ifndef _RWSTD_NO_VA_LIST_ARRAY
   // va_list is an array type
#  define RW_VA_LIST_ARG_PTR                 va_list
#  define RW_VA_LIST_ARG_TO_PTR(va)          va
#  define RW_VA_LIST_PTR_ARG_TO_VA_LIST(va)  va
#else   // if defined (_RWSTD_NO_VA_LIST_ARRAY)
   // va_list is an object type
#  define RW_VA_LIST_ARG_PTR                 va_list*
#  define RW_VA_LIST_ARG_TO_PTR(va)          (va_list *)(&va)
#  define RW_VA_LIST_PTR_ARG_TO_VA_LIST(pva) *pva
#endif   // _RWSTD_NO_VA_LIST_ARRAY


static const char*
_rw_getbounds (const char *next, char sep, RW_VA_LIST_ARG_PTR pva)
{
    ++next;

    if (   '*' == *next || '+' == *next || '-' == *next || sep == *next
        || isdigit (*next)) {

        bool have_maxval = false;

        if (*next == sep && '-' != sep) {
            have_maxval = true;
            ++next;
        }

        char *end = 0;

        // '*' designates an int va_arg argument
        const long val = '*' == *next ?
              long (va_arg (RW_VA_LIST_PTR_ARG_TO_VA_LIST (pva), int))
            : strtol (next, &end, 10);

#if _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE
        // validate
        if (val < _RWSTD_INT_MIN || _RWSTD_INT_MAX < val) {
            fprintf (stderr,
                     "lower bound %ld out of range [%d, %d]: "
                     "%s\n", val, _RWSTD_INT_MIN, _RWSTD_INT_MAX,
                     next);
            return 0;
        }
#endif   // INT_SIZE < LONG_SIZE

        next = end ? end : next + 1;

        if (have_maxval) {
            _rw_cmdopts [_rw_ncmdopts].minval_ = 0;
            _rw_cmdopts [_rw_ncmdopts].maxval_ = val;
        }
        else
            _rw_cmdopts [_rw_ncmdopts].minval_ = val;

        if (sep == *next && !have_maxval) {
            ++next;

            if (   '*' == *next
                || '+' == *next
                || val < 0 && '-' == *next
                || isdigit (*next)) {

                end = 0;

                // '*' designates an int va_arg argument
                const long maxval = '*' == *next ?
                      long (va_arg (RW_VA_LIST_PTR_ARG_TO_VA_LIST (pva), int))
                    : strtol (next, &end, 10);

#if _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE
                // validate
                if (maxval < _RWSTD_INT_MIN || _RWSTD_INT_MAX < maxval) {

                    fprintf (stderr,
                             "upper bound %ld out of range [%ld, %d]: "
                             "%s\n", maxval, val, _RWSTD_INT_MAX,
                             next);
                    return 0;
                }
#endif   // INT_SIZE < LONG_SIZE

                if ('-' == sep && maxval < val) {
                    // invalid range (upper bound < lower bound
                    fprintf (stderr,
                             "invalid range [%ld, %ld]: %s\n",
                             val, maxval, next);
                    return 0;
                }

                next = end ? end : next + 1;
                _rw_cmdopts [_rw_ncmdopts].maxval_ = int (maxval);
            }
            else {
                // syntax error in range
                fprintf (stderr,
                         "syntax error in range specification: %s\n",
                         next);
                return 0;
            }
        }
        else if (*next && !isspace (*next)) {
            // syntax error in numeric argument
            fprintf (stderr,
                     "syntax error numeric argument: %s\n",
                     next);
            return 0;
        }
        else if (!have_maxval) {
            // no upper bound on the value of the option argument
            _rw_cmdopts [_rw_ncmdopts].maxval_ = _RWSTD_INT_MAX;
        }
    }
    else if (':' == sep) {
        // special value indicating no bits specified for a tristate
        _rw_cmdopts [_rw_ncmdopts].minval_ = 0;
        _rw_cmdopts [_rw_ncmdopts].maxval_ = 0;
    }
    else {
        // no minimum/maximum value for this option is set
        _rw_cmdopts [_rw_ncmdopts].minval_ = _RWSTD_INT_MIN;
        _rw_cmdopts [_rw_ncmdopts].maxval_ = _RWSTD_INT_MAX;
    }

    // an unlimited number of occurrences of the option
    // are allowed and will be counted
    _rw_cmdopts [_rw_ncmdopts].maxcount_ = _RWSTD_SIZE_MAX;

    return next;
}


_TEST_EXPORT int
rw_vsetopts (const char *opts, va_list va)
{
    if (0 == opts) {

        _rw_clear_opts ();
        return 0;
    }

    _rw_set_myopts ();

    const char *next = opts;

    for ( ; ; ++_rw_ncmdopts) {

        while (isspace (*next))
            ++next;

        if ('\0' == *next) {
            break;
        }

        if (_rw_ncmdopts == _rw_optbufsize) {

            const size_t newbufsize = 2 * _rw_ncmdopts + 1;

            cmdopts_t* const newopts =
                (cmdopts_t*)malloc (newbufsize * sizeof (cmdopts_t));

            if (0 == newopts) {
                fprintf (stderr, "%s%d: failed to allocate memory\n",
                         __FILE__, __LINE__);
                abort ();
            }

            memcpy (newopts, _rw_cmdopts, _rw_ncmdopts * sizeof (cmdopts_t));

            if (_rw_cmdopts != _rw_cmdoptbuf)
                free (_rw_cmdopts);

            _rw_cmdopts    = newopts;
            _rw_optbufsize = newbufsize;
        }

        // clear the next option info
        memset (_rw_cmdopts + _rw_ncmdopts, 0, sizeof *_rw_cmdopts);

        // for convenience
        cmdopts_t* const lastopt = _rw_cmdopts + _rw_ncmdopts;

        lastopt->minval_ = _RWSTD_INT_MIN;
        lastopt->maxval_ = _RWSTD_INT_MAX;

        if ('|' != *next)
            lastopt->sopt_ = *next++;

        if ('|' == *next) {
            const char* end = strpbrk (++next, "|@:=*!#~ ");
            if (0 == end)
                end = next + strlen (next);

            // copy the option name up to but not including the delimiter
            // (except when the delimiter is the equals sign ('='), which
            // becomes the last character of the option name
            const size_t optlen = size_t (end - next) + ('=' == *end);

            char *lopt = 0;

            if (optlen < sizeof lastopt->loptbuf_)
                lopt = lastopt->loptbuf_;
            else {
                lopt = (char*)malloc (optlen + 1);
                if (0 == lopt)
                    return -1;   // error
                lastopt->lopt_ = lopt;
            }

            memcpy (lopt, next, optlen);
            lopt [optlen] = '\0';

            next = end;
        }

        // only the first occurrence of each command line option
        // causes an invocation of the callback, all subsequent
        // ones will be ignored by default
        lastopt->maxcount_ = 1;

        int arg_is_callback = true;

        RW_ASSERT (0 != next);

        if ('#' == *next) {
            // examples of option specification:
            //   --foo#     option takes an optional numeric argument
            //   --foo#3    numeric argument must be equal to 3
            //   --foo#1-5  numeric argument must be in the range [1,5]

            // instead of a pointer to a callback, the argument is a pointer
            // to an int counter that is to be incremented for each occurrence
            // of the option during processing
            // when the option is immediately followed by the equals sign ('=')
            // and a numeric argument N, the value of N will be stored instead
            next = _rw_getbounds (next, '-', RW_VA_LIST_ARG_TO_PTR (va));

            if (0 == next)
                return -1;   // error

            arg_is_callback = false;

        }
        else if ('~' == *next) {
            // examples of option specification:
            //   --foo~      word 0
            //   --foo~3     word 3
            //   --foo~:4    word 0, bit 2
            //   --foo~3:6   word 3, bits 2 and 3

            // get the optional word number and bit number
            next = _rw_getbounds (next, ':', RW_VA_LIST_ARG_TO_PTR (va));

            if (0 == next)
                return -1;   // error

            // unlimited number of tristates are allowed
            lastopt->tristate_ = 1;

            // no callback function expected
            arg_is_callback = false;
        }
        else if (':' == *next || '=' == *next) {
            // ':' : argument optional
            // '=' : argument required
            lastopt->arg_ = true;

            // check if the value of the argument is restricted
            next = _rw_getbounds (next, '-', RW_VA_LIST_ARG_TO_PTR (va));

            if (0 == next)
                return -1;   // error
        }

        if ('@' == *next) {

            ++next;

            // at most how many occurrences of an option can be processed?
            if ('*' == *next) {
                // unlimited
                lastopt->maxcount_ = _RWSTD_SIZE_MAX;
                ++next;
            }
            else {
                // at most this many
                char *end;
                lastopt->maxcount_ = strtoul (next, &end, 10);
                next = end;
            }
        }
        else if ('!' == *next) {
            lastopt->inv_ = true;
            ++next;
        }

        if (arg_is_callback) {
            // retrieve the callback and verify it's not null
            // (null callback is permitted in the special case when
            // the short option is '-', i.e., when setting up or
            // resetting an "unknown option" handler)
            lastopt->callback_ = va_arg (va, optcallback_t*);
        }
        else {
            // retrieve the address of the int counter where to keep
            // track of the number of occurrences of the option, or
            // where to store the value of the numeric argument of
            // the option
            lastopt->pcntr_ = va_arg (va, int*);
        }

        if (   '-' != lastopt->sopt_
            && 0 == lastopt->callback_
            && 0 == lastopt->pcntr_) {

            // get a pointer to the long option name
            const char* const lopt = lastopt->lopt_
                ? lastopt->lopt_ : lastopt->loptbuf_;

            if (*lopt)
                fprintf (stderr, "null handler for option -%s\n", lopt);
            else
                fprintf (stderr, "null handler for option -%c\n",
                         lastopt->sopt_);

            abort ();
        }
    }

    return int (_rw_ncmdopts - _rw_ndefopts);
}

/**************************************************************************/

_TEST_EXPORT int
rw_setopts (const char *opts, ...)
{
    va_list va;
    va_start (va, opts);
    const int result = rw_vsetopts (opts, va);
    va_end (va);
    return result;
}

/**************************************************************************/

// validates (and optionally stores) a restricted numeric option argument
static int
_rw_getarg (cmdopts_t *optspec, const char *opt, const char *arg)
{
    RW_ASSERT (0 != optspec);
    RW_ASSERT (0 != arg);

    // obtain the numeric argument
    char *end = 0;
    const long optval = strtol (arg, &end, 0);

    int status = 0;

    if (end && '\0' != *end) {
        fprintf (stderr, "expected numeric argument: %s\n", opt);
        errno  = EINVAL;
        status = 1;
    }

#if _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE

    else if (optval < _RWSTD_INT_MIN || _RWSTD_INT_MAX < optval) {
        fprintf (stderr, "numeric argument %ld out of range"
                 " [%d, %d]: %s\n", optval,
                 _RWSTD_INT_MIN, _RWSTD_INT_MAX, opt);
        errno  = EINVAL;
        status = 1;
    }

#endif   // _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE

    else if (   optval < long (optspec->minval_)
             || long (optspec->maxval_) < optval) {

        // numeric argument out of range
        fprintf (stderr, "numeric argument %ld out of range"
                 " [%d, %d]: %s\n", optval,
                 optspec->minval_, optspec->maxval_, opt);
        errno  = EINVAL;
        status = 1;
    }
    else if (optspec->pcntr_) {

        // set the counter
        *optspec->pcntr_ = optval;
    }

    return status;
}

/**************************************************************************/

static bool
_rw_runopt_recursive = false;


// processes a single command line option and its arguments
static int
_rw_runopt (cmdopts_t *optspec, int argc, char *argv[])
{
    RW_ASSERT (0 != optspec);
    RW_ASSERT (0 != argv);

    // ignore the option if invoked recursively (by processing options
    // set in the environment) and the option has already been seen
    // (this prevents duplicate processing of options that are set both
    // on the command line and in the environment and allows option with
    // an argument set on the command line to override those set in the
    // environment)

    if (optspec->count_ && _rw_runopt_recursive)
        return 0;

    // if the option has been evaluated the maximum number
    // of times, avoid evaluating it and continue processing
    if (optspec->maxcount_ <= optspec->count_)
        return 0;

    int status = 0;

    char* equals = strchr (argv [0], '=');

    if (optspec->callback_) {
        // option matched the specification

        if (!optspec->inv_) {
            // option is not inverted (i.e., is processed when seen
            // on the command line as opposed processed when missing)

            const char* const lopt = optspec->lopt_ ?
                optspec->lopt_ : optspec->loptbuf_;

            if (   optspec->arg_
                && (   _RWSTD_INT_MIN < optspec->minval_
                    || optspec->maxval_ < _RWSTD_INT_MAX)) {

                if (equals) {
                    if (strchr (lopt, '=')) {
                        // option requires a restricted numeric argument
                        // retrieve and validate it before invoking the
                        // callback

                        status = _rw_getarg (optspec, argv [0], equals + 1);
                    }
                }
                else if (1 < argc && '-' != argv [1][0]) {

                    const char *arg = argv [1];

                    // handle escaped minus (necessary to disambiguate
                    // it from a leading dash that introduces an option)
                    if ('\\' == arg [0] && '-' == arg [1])
                        ++arg;

                    status = _rw_getarg (optspec, argv [0], arg);
                }
            }

            if (0 == status)
                status = optspec->callback_ (argc, argv);
        }
    }
    else if (equals) {
        // option takes an optional numeric argument

        RW_ASSERT (0 != optspec->pcntr_);

        status = _rw_getarg (optspec, argv [0], equals + 1);
    }
    else {
        // option must not be a tristate (those are handled elsewhere)
        RW_ASSERT (0 == optspec->tristate_);
        RW_ASSERT (0 != optspec->pcntr_);
        ++*optspec->pcntr_;
    }

    ++optspec->count_;

    if (_rw_runopt_recursive)
        optspec->envseen_ = true;

    return status;
}

/**************************************************************************/

// tries to match the option named by optname with the option
// specification opt and returns a non-zero value on success,
// 0 otherwise; the returned value is negative when the matched
// option is being disabled and positive when it's being
// enabled
static int
_rw_match_tristate (const cmdopts_t *opt, const char *optname)
{
    RW_ASSERT (0 != opt);
    RW_ASSERT (0 != optname);

    static const char* const prefix[] = {
        "+enable", "+use", "+with",
        "-disable", "-no", "-without",
        0
    };

    int tristate = 0;

    if ('-' == optname [0] && '-' != optname [2]) {

        ++optname;

        const size_t optlen = strlen (optname);

        for (size_t i = 0; prefix [i]; ++i) {
            const char* const pfx    = prefix [i] + 1;
            const size_t      pfxlen = strlen (pfx);

            if (pfxlen < optlen && 0 == memcmp (optname, pfx, pfxlen)) {
                const char* const name = optname + pfxlen;

                // return -1 to disable, +1 to enable, 0 when not found
                const char* const lopt =
                    opt->lopt_ ? opt->lopt_ : opt->loptbuf_;

                if (0 == strcmp (lopt, name)) {
                    tristate = '+' == prefix [i][0] ? 1 : -1;
                    break;
                }
            }
        }
    }

    return tristate;
}

/**************************************************************************/

// returns a bitmap all of whose bits are set starting with the most
// significant non-zero bit corresponding to first such bit in val
// e.g., _rw_set_bits(4) --> 7
static int
_rw_set_bits (int val)
{
    int res = 0;

    for (unsigned uval = val; uval; uval >>= 1) {
        res = (res << 1) | 1;
    }

    return res;
}


_TEST_EXPORT int
rw_runopts (int argc, char *argv[])
{
    _rw_set_myopts ();

    // ignore options set in the environment?
    int ignenv = _rw_runopt_recursive;

    // return status
    int status = 0;

    // number of options processed
    int nopts = 0;

    // pointer to the option whose callback will be invoked
    // for command line options that do not match any other
    cmdopts_t* not_found_opt = 0;

    // iterate over the command line arguments until a callback
    // returns a non-zero value or until all options have been
    // successfully processed
    for (int i = 0; i < argc && argv [i] && 0 == status; ++i) {

        if (0 == strcmp ("--ignore-environment", argv [i])) {
            // ignore options set in the environment
            ignenv = true;
            continue;
        }

        if (0 == strcmp ("--", argv [i])) {
            // "--" terminates options, everything
            // after it is treated as an argument
            break;
        }

        // the name of the option without the leading dash
        const char* const optname = argv [i] + 1;

        // the argc and argv to pass to the option handler
        const int    opt_argc = argc - i;
        char** const opt_argv = argv + i;

        // look for the first equals sign
        const char* const eq = strchr (optname, '=');

        // compute the length of the option including the equals sign (if any)
        const size_t optlen = eq ? size_t (eq - optname + 1) : strlen (optname);

        int found = false;

        // look up each command line option (i.e., a string that starts
        // with a dash ('-')) and invoke the callback associated with it
        for (size_t j = 0; j != _rw_ncmdopts; ++j) {

            // for convenience
            cmdopts_t* const opt = _rw_cmdopts + j;

            if ('-' == opt->sopt_)
                not_found_opt = opt;

            if ('-' == argv [i][0]) {

                const size_t cmplen =
                    eq && opt->pcntr_ ? optlen - 1 : optlen;

                // get a pointer to the (possibly empty) name
                // of the long option
                const char* const lopt = opt->lopt_ ?
                    opt->lopt_ : opt->loptbuf_;

                if (opt->tristate_) {
                    // option specification denotes a tristate, see if it
                    // matches and if so, whether it's being disabled or
                    // enabled (-1 or +1, respectively)
                    const int tristate = _rw_match_tristate (opt, optname);

                    if (tristate) {
                        RW_ASSERT (0 != opt->pcntr_);

                        const int word_bits = _RWSTD_CHAR_BIT * sizeof (int);

                        // the optional minval_ specifies the bit number
                        // of the first of a sequence of bits in an array
                        // stored at pcntr_
                        const int word_inx = opt->minval_ / word_bits;
                        const int bit_inx  = opt->minval_ % word_bits;

                        const int bit_val = _RWSTD_INT_MAX == opt->maxval_
                            ? 1 : opt->maxval_;

                        if (bit_val) {
                            const int bit_mask =
                                _rw_set_bits (bit_val) << bit_inx;

                            // clear the bit field
                            opt->pcntr_ [word_inx] &= ~bit_mask;

                            if (tristate < 0)
                                opt->pcntr_ [word_inx] = ~(bit_val << bit_inx);
                            else
                                opt->pcntr_ [word_inx] |= bit_val << bit_inx;
                        }
                        else {
                            // special case
                            opt->pcntr_ [word_inx] = tristate < 0 ? -1 : +1;
                        }

                        // matching option has been found
                        found = true;

                        // increment the number of options processed
                        ++nopts;
                    }

                    // avoid ordinary option processing below
                    continue;
                }

                // try to match the long option first, and only if it
                // doesn't match try the short single-character option
                if (   cmplen == strlen (lopt)
                    && 0 == memcmp (optname, lopt, cmplen)
                    || opt->sopt_
                    && optname [0] == opt->sopt_
                    && (1 == optlen || opt->arg_)) {

                    // matching option has been found
                    found = true;

                    // process it and its arguments, if any
                    status = _rw_runopt (opt, opt_argc, opt_argv);

                    // increment the number of options processed
                    // (whether successfully or otherwise)
                    ++nopts;

                    if (status) {
                        // when the status returned from the last callback
                        // is non-0 stop further processing (including
                        // any options set in the environment) and return
                        // status to the caller
                        ignenv = true;
                        break;
                    }
                }
            }
        }

        if (!found && '-' == argv [i][0]) {

            // invoke the appropriate error handler for an option
            // that was not found
            if (0 != not_found_opt) {

                // invoke the error handler set up through rw_setopts()
                // and let the handler decide whether to go on processing
                // other options or whether to abort
                status = not_found_opt->callback_ (opt_argc, opt_argv);
                if (status) {
                    // no further processing done
                    ignenv = true;
                    break;
                }
            }
            else {
                // print an error message to stderr when no error
                // handler has been set up
                fprintf (stderr, "unknown option: %s\n", argv [i]);
                ignenv = true;
                errno  = EINVAL;
                status = 1;
                break;
            }
        }
    }

    if (!ignenv) {
        // process options from the environment
        const char* const envar = getenv ("RWSTD_TESTOPTS");
        if (envar) {
            _rw_runopt_recursive = true;
            rw_runopts (envar);
            _rw_runopt_recursive = false;
        }
    }

    // invoke any inverted callbacks or bump their user-specified counters,
    // and reset internal counters indicating if/how many times each option
    // has been processed
    for (size_t j = 0; j != _rw_ncmdopts; ++j) {

        // for convenience
        cmdopts_t* const opt = _rw_cmdopts + j;

        if (opt->inv_ && 0 == opt->count_ && 0 == status) {

            if (opt->callback_)
                status = opt->callback_ (0, 0);
            else {
                RW_ASSERT (0 != opt->pcntr_);
                ++*opt->pcntr_;
            }
        }

        opt->count_   = 0;
        opt->envseen_ = false;
    }

    return status;
}

/**************************************************************************/

_TEST_EXPORT int
rw_runopts (const char *str)
{
    RW_ASSERT (0 != str);

    _rw_set_myopts ();

    char buf [80];      // fixed size buffer to copy `str' into
    char *pbuf = buf;   // a modifiable copy of `str'

    size_t len = strlen (str);
    if (len >= sizeof buf) {
        // allocate if necessary
        pbuf = (char*)malloc (len + 1);
        if (!pbuf)
            return -1;
    }

    // copy `str' to modifiable buffer
    memcpy (pbuf, str, len + 1);

    char *tmp_argv_buf [32] = { 0 };   // fixed size argv buffer
    char **argv = tmp_argv_buf;        // array of arguments

    // initial size of argument array (will grow as necessary)
    size_t argv_size = sizeof tmp_argv_buf / sizeof *tmp_argv_buf;
    size_t argc = 0;   // number of arguments in array

    int in_quotes = 0;   // quoted argument being processed

    for (char *s = pbuf; *s; ++s) {
        if ('"' == *s) {
            in_quotes = !in_quotes;
            continue;
        }

        if (in_quotes)
            continue;

        // split up unquoted space-separated arguments
        if (argc == 0 || isspace (*s)) {
            if (argc > 0)
                *s = 0;

            // skip over leading spaces
            if (argc > 0 || isspace (*s))
                while (isspace (*++s));

            if (*s) {
                if (argc == argv_size) {
                    // grow `argv' as necessary
                    char **tmp = (char**)malloc (sizeof *tmp * argv_size * 2);
                    if (!tmp) {
                        if (argv != tmp_argv_buf)
                            free (argv);
                        return -1;
                    }

                    // copy existing elementes and zero out any new entries
                    memcpy (tmp, argv, sizeof *tmp * argv_size);
                    memset (tmp + argv_size, 0, sizeof *tmp * argv_size);

                    // free existing buffer if necessary
                    if (argv != tmp_argv_buf)
                        free (argv);

                    // reassign buffer and increase size
                    argv       = tmp;
                    argv_size *= 2;
                }

                // add argument to array
                argv [argc++] = s;
            }
        }
    }

    // process `argc' options pointed to by `argv'
    const int status = rw_runopts (int (argc), argv);

    // free buffers if necessary
    if (argv != tmp_argv_buf)
        free (argv);

    if (pbuf != buf)
        free (pbuf);

    return status;
}
