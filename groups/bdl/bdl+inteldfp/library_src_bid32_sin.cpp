#include <bsls_platform.h>

#if 1

#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wcast-qual"

#ifdef __GNUC__
#  define _WCHAR_T
#elif !defined(BSLS_PLATFORM_OS_WINDOWS)
#  define __thread
#  define __QNX__
#endif

#ifndef BSLS_PLATFORM_OS_WINDOWS
#  define LINUX
#  define efi2
#endif

#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
#define BID_BIG_ENDIAN 1
#else
#define BID_BIG_ENDIAN 0
#endif


extern "C" {
  #include "LIBRARY/src/bid32_sin.c"
}

#  pragma GCC diagnostic pop

#endif
