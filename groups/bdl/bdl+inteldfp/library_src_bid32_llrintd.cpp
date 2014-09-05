#include <bsls_platform.h>

#ifndef BSLS_PLATFORM_OS_WINDOWS

#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wcast-qual"

#ifdef __GNUC__
#  define _WCHAR_T
#else
#  define __thread
#  define __QNX__
#endif


extern "C" {
  #include "LIBRARY/src/bid32_llrintd.c"
}

#  pragma GCC diagnostic pop

#endif
