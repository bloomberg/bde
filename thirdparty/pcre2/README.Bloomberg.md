Changes to PCRE2 to adapt to the BDE Repo and Build Structure
=============================================================

This directory contains an almost-unmodified copy of pcre2 version 10.20.

However, a few minor changes were applied.

Build System Changes
====================

Created a `wscript` file to build using waf, based on instructions from the
`NON-AUTOTOOLS-BUILD` file.


VS2008 Support Changes
======================

In order to allow building on the older Visual Studio 2008 compiler, a minor
patch had to be applied to pcre2.h to work around VS2008''s lack of stdint.h.

    $ git diff e12324e4cb83c16642fd230027fb29c6705fc026..13b1728eed65d463d906a597b6d93ee6b38a6654
    diff --git a/thirdparty/pcre2/pcre2.h b/thirdparty/pcre2/pcre2.h
    index 3e97fb8..1f397bc 100644
    --- a/thirdparty/pcre2/pcre2.h
    +++ b/thirdparty/pcre2/pcre2.h
    @@ -72,7 +72,19 @@ uint8_t, UCHAR_MAX, etc are defined. */

     #include <limits.h>
     #include <stdlib.h>
    +
    +/* Prior to VS2010, Visual Studio did not provide stdint.h.  In that case,
    + * manually provide the needed typedef's and #define. */
    +#if _MSC_VER >= 1600
     #include <stdint.h>
    +#else
    +typedef unsigned char  uint8_t;
    +typedef unsigned short uint16_t;
    +typedef signed int     int32_t;
    +typedef unsigned int   uint32_t;
    +
    +#define UINT32_MAX     0xFFFFFFFFU
    +#endif

     /* Allow for C++ users compiling this directly. */
