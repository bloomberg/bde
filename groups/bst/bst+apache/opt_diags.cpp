/************************************************************************
 *
 * opt_diags.cpp - definitions of diagnostic option handlers
 *
 * $Id: opt_diags.cpp 650748 2008-04-23 05:15:10Z sebor $
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include "opt_diags.h"

#include <assert.h>   // for assert()
#include <stdio.h>    // for fprintf()
#include <string.h>   // for strlen(), strncmp(), ...


#define ESC "\x1b"
#define ESC_END ESC "[30;30;0m"
#define ESC_CODE(fg, bg, attr)   ESC "[" fg bg attr "m", ESC_END


// ANSI VT100 terminal codes:
#define AT_OFF   "0;"   // all attributes off
#define AT_BLD   "1;"   // bright/bold
#define AT_DIM   "2;"   // dim
// #define AT_???   "3;"   // ???
#define AT_UND   "4;"   // underscore
#define AT_BLI   "5;"   // blink
// #define AT_???   "6;"   // ???
#define AT_RVS   "7;"   // reverse video
#define AT_HID   "8;"   // hidden

#define FG_BLK   "30;"   // foreground black
#define FG_RED   "31;"   // foreground red
#define FG_GRN   "32;"   // foreground green
#define FG_YLW   "33;"   // foreground yellow
#define FG_BLU   "34;"   // foreground blue
#define FG_MAG   "35;"   // foreground magenta
#define FG_CYN   "36;"   // foreground cyan
#define FG_WHT   "37;"   // foreground white

#define BG_BLK   "40;"   // background black
#define BG_RED   "41;"   // background red
#define BG_GRN   "42;"   // background green
#define BG_YLW   "43;"   // background yellow
#define BG_BLU   "44;"   // background blue
#define BG_MAG   "45;"   // background magenta
#define BG_CYN   "46;"   // background cyan
#define BG_WHT   "47;"   // background white


esc_text_t diag_msgs[] = {
    /* severity 0 */ {
        ESC_CODE (FG_CYN, "", AT_DIM),
        "ITRACE",
        "inactive diagnostic trace"
    },
    /* severity 1 */ {
        ESC_CODE (FG_BLK, "",     ""),
        "INFO",
        "information"
    },
    /* severity 2 */ {
        ESC_CODE (FG_GRN, "", AT_BLI),
        "NOTE",
        "noteworthy event"
    },
    /* severity 3 */ {
        ESC_CODE (FG_WHT, BG_GRN, AT_BLD),
        "EXPECT",
        "expected diagnostic inactive"
    },
    /* severity 4 */ {
        ESC_CODE (FG_MAG, "", ""),
        "XWARN",
        "expected warning"
    },
    /* severity 5 */ {
        ESC_CODE (FG_MAG, "", AT_BLD AT_UND),
        "WARNING",
        ""
    },
    /* severity 6 */ {
        ESC_CODE (FG_RED, "", ""),
        "XASSERT",
        "expected assertion"
    },
    /* severity 7 */ {
        ESC_CODE (FG_RED, "", AT_BLD AT_UND),
        "ASSERTION",
        ""
    },
    /* severity 8 */ {
        ESC_CODE (FG_WHT, BG_RED, AT_BLD),
        "ERROR",
        "test error"
    },
    /* severity 9 */ {
        ESC_CODE (FG_YLW, BG_RED, AT_BLD),
        "FATAL",
        "fatal test error, abort imminent"
    }
};


static const esc_text_t
_rw_vt100_colors[] = {
    // prefix suffix   code       description
    { FG_BLK, ESC_END, "black",   "" },
    { FG_RED, ESC_END, "red",     "" },
    { FG_GRN, ESC_END, "green",   "" },
    { FG_YLW, ESC_END, "yellow",  "" },
    { FG_BLU, ESC_END, "blue",    "" },
    { FG_MAG, ESC_END, "magenta", "" },
    { FG_CYN, ESC_END, "cyan",    "" },
    { FG_WHT, ESC_END, "white",   "" }
};


static const esc_text_t
_rw_vt100_attribs[] = {
    // prefix suffix   code          description
    { AT_OFF, ESC_END, "off",        "" },
    { AT_BLD, ESC_END, "bold",       "" },
    { AT_DIM, ESC_END, "dim",        "" },
    { AT_UND, ESC_END, "underscore", "" },
    { AT_BLI, ESC_END, "blink",      "" },
    { AT_RVS, ESC_END, "reverse",    "" },
    { AT_HID, ESC_END, "hidden",     "" }
};


static int
_rw_match_name (const esc_text_t *text, size_t nelems,
                const char *first, const char *last)
{
    assert (0 != text);
    assert (0 != first);
    assert (first <= last);

    const size_t len = size_t (last - first);

    if (0 == len)
        return int (nelems);

    for (size_t i = 0; i != nelems; ++i) {
        if (   0 == strncmp (text [i].code, first, len)
            && '\0' == text [i].code [len])
            return int (i);
    }

    return -1;
}


/* extern */ int
_rw_setopt_diags (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Sets the colors and names of the diagnostic messages issued by\n"
            "the program.\n"
            "There are 10 different types of diagnostic messages, each with\n"
            "a unique severity level between 0 (the lowest) and 9. Each\n"
            "diagnostic message can either be active or inactive. Each\n"
            "message can have a name of up to 15 characters associated with\n"
            "it, foreground color, background color, and a video attribute.\n"
            "These parameters are controlled by the argument to this option.\n"
            "\nThe syntax of <arg> is as follows:\n"
            "<arg>        ::= <color-list>\n"
            "<color-list> ::= <color-txt> [ ,<color-list> ]\n"
            "<color-txt>  ::= <sev>:[<color>][:[<color>][:[<attr>][:[<text>]]]]"
            "\n<sev>        ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9\n"
            "<color>      ::=   black | red | green | yellow\n"
            "                 | blue | magenta | cyan | white\n"
            "<attr>       ::=   off | bold | dim | underscore\n"
            "                 | blink | reverse | hidden\n"
            "<text>       ::= A-Z a-z 0-9 _\n\n"
            "The first <color> component specifies the foreground color\n"
            "while the second (optional) <color> component specifies\n"
            "the background color for the diagnostic of the severity <sev>.\n"
            "Empty arguments are permitted and denote the default system\n"
            "color set for the terminal.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    char *parg = strchr (argv [0], '=');

    if (0 == parg || '\0' == parg [1]) {
        fprintf (stderr, "%s:%d: missing argument in %s\n",
                 __FILE__, __LINE__, argv [0]);
        return 1;
    }

    ++parg;

    // argument syntax:
    //
    // <sev-color-list> := <sev-color-text> [ ,<sev-color-list> ]
    // <sev-color-text> := <sev>:[<color>][:[<color>][:[<attr>][:[<text>]]]]
    // <sev>            := 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
    // <color>          :=   black | red | green | yellow
    //                     | blue | magenta | cyan | white
    // <attr>           :=   off | bold | dim | underscore
    //                     | blink | reverse | hidden
    // <text>           := A-Z a-z 0-9 _


    int ret = 0;

    while (*parg) {

        int sev = -1;
        int fgcol    = -1;
        int bgcol    = -1;
        int attr     = -1;

        char diag_code [sizeof diag_msgs [0].code];
        *diag_code = '\0';

        if ('0' <= *parg && *parg <= '9') {
            sev = *parg++ - '0';
        }
        else {
            // digit expected
            fprintf (stderr, "%s:%d: digit expected at position %d: %s\n",
                     __FILE__, __LINE__, int (parg - argv [0]), argv [0]);

            ret = 1;
            break;
        }

        if (':' == *parg) {
            ++parg;
        }
        else {
            // colon expected
            fprintf (stderr, "%s:%d: colon expected at position %d: %s\n",
                     __FILE__, __LINE__, int (parg - argv [0]), argv [0]);

            ret = 1;
            break;
        }

        char *end = strpbrk (parg, ":,");
        if (0 == end)
            end = parg + strlen (parg);

        fgcol = _rw_match_name (_rw_vt100_colors, 8, parg, end);
        if (fgcol < 0 || 8 < fgcol) {
            // invalid color
            fprintf (stderr, "%s:%d: unknown color at position %d: %s\n",
                     __FILE__, __LINE__, int (parg - argv [0]), argv [0]);

            ret = 1;
            break;
        }

        RW_ASSERT (0 != end);

        if (':' == *end || '\0' == *end) {
            parg = end + (0 != *end);
            end  = strpbrk (parg, ":,");
            if (0 == end)
                end = parg + strlen (parg);

            bgcol = _rw_match_name (_rw_vt100_colors, 8, parg, end);

            if (bgcol < 0 || 8 < bgcol) {
                // invalid color
                fprintf (stderr, "%s:%d: unknown color at position %d: %s\n",
                         __FILE__, __LINE__, int (parg - argv [0]), argv [0]);

                ret = 1;
                break;
            }

            RW_ASSERT (0 != end);

            if (':' == *end || '\0' == *end) {
                parg = end + ('\0' != *end);
                end  = strpbrk (parg, ":,");
                if (0 == end)
                    end = parg + strlen (parg);

                attr = _rw_match_name (_rw_vt100_attribs, 8, parg, end);
                if (attr < 0 || 8 < attr) {
                    // invalid attribute
                    fprintf (stderr,
                             "%s:%d: unknown attribute at position %d: %s\n",
                             __FILE__, __LINE__, int (parg - argv [0]),
                             argv [0]);

                    ret = 1;
                    break;
                }

                RW_ASSERT (0 != end);

                if (':' == *end || '\0' == *end) {
                    parg = end + (0 != *end);
                    end  = strpbrk (parg, ":,");
                    if (0 == end)
                        end = parg + strlen (parg);

                    size_t len = size_t (end - parg);

                    if (sizeof diag_msgs [sev].code < len) {

                        // name too long
                        fprintf (stderr,
                                 "%s:%d: name too long at position %d: %s\n",
                                 __FILE__, __LINE__, int (parg - argv [0]),
                                 argv [0]);

                        len = strlen (diag_msgs [sev].code);
                    }

                    memcpy (diag_code, parg, len);
                    diag_code [len] = '\0';
                }
            }
        }

        strcpy (diag_msgs [sev].esc_pfx, ESC "[");

        if (-1 < fgcol && fgcol < 8)
            strcat (diag_msgs [sev].esc_pfx, _rw_vt100_colors [fgcol].esc_pfx);

        if (-1 < bgcol && bgcol < 8) {
            strcat (diag_msgs [sev].esc_pfx, _rw_vt100_colors [bgcol].esc_pfx);

            const size_t bgdiginx = strlen (diag_msgs [sev].esc_pfx) - 3;

            assert ('3' == diag_msgs [sev].esc_pfx [bgdiginx]);
            diag_msgs [sev].esc_pfx [bgdiginx] = '4';
        }

        if (-1 < attr && attr < 8)
            strcat (diag_msgs [sev].esc_pfx, _rw_vt100_attribs [attr].esc_pfx);

        if (diag_msgs [sev].esc_pfx [2]) {
            strcat (diag_msgs [sev].esc_pfx, "m");
            strcpy (diag_msgs [sev].esc_sfx, ESC_END);
        }
        else {
            diag_msgs [sev].esc_pfx [0] = '\0';
            diag_msgs [sev].esc_sfx [0] = '\0';
        }

        if (*diag_code)
            strcpy (diag_msgs [sev].code, diag_code);

        parg = end + ('\0' != *end);
    }

    return ret;
}
