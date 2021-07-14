Changes to PCRE2 to adapt to the BDE Repo and Build Structure
=============================================================

This directory contains an almost-unmodified copy of pcre2 version 10.37.

However, a few minor build system changes were applied, and a patch was applied
for 64-bit AIX builds.

Build System Changes
====================

Created a `package/pcre2.cmake' file to build using CMake.

AIX Builds (64-bit)
===================

The following patch was applied.  Note that this patch will no longer be
necessary when the [upstream patch](https://bugs.exim.org/show_bug.cgi?id=2787)
lands.

```
From c875317fe84f3c9904ce9c940ab7ac2eb8e41ac2 Mon Sep 17 00:00:00 2001
From: Mike Giroux <mgiroux@bloomberg.net>
Date: Mon, 12 Jul 2021 11:31:06 -0400
Subject: [PATCH] Fix sljit platform detection with xlc on AIX (#3329)

The XL C compiler's _ARCH_* macros are based on the -qarch setting and
do not necessarily imply the object mode. In particular the _ARCH_PPC64
macro only indicates that the compiler is targeted to run on a Power
processor with 64-bit support. The __64BIT__ macro is needed to detect
that the compiler is actually in 64bit mode.

This reapplies commit f417f3333b2212f19d23554ccb7802d3be3227b3
Author: Jeremy Devenport <jdevenport@bloomberg.net>
---
 thirdparty/pcre2/sljit/sljitConfigInternal.h | 2 +-
 1 file changed, 1 insertion(+), 1 deletion(-)

diff --git a/thirdparty/pcre2/sljit/sljitConfigInternal.h b/thirdparty/pcre2/sljit/sljitConfigInternal.h
index ff36e5b7c6..1bc3b570b8 100644
--- a/thirdparty/pcre2/sljit/sljitConfigInternal.h
+++ b/thirdparty/pcre2/sljit/sljitConfigInternal.h
@@ -148,7 +148,7 @@ extern "C" {
 #endif
 #elif defined (__aarch64__)
 #define SLJIT_CONFIG_ARM_64 1
-#elif defined(__ppc64__) || defined(__powerpc64__) || defined(_ARCH_PPC64) || (defined(_POWER) && defined(__64BIT__))
+#elif defined(__ppc64__) || defined(__powerpc64__) || (defined(_ARCH_PPC64) && defined(__64BIT__)) || (defined(_POWER) && defined(__64BIT__))
 #define SLJIT_CONFIG_PPC_64 1
 #elif defined(__ppc__) || defined(__powerpc__) || defined(_ARCH_PPC) || defined(_ARCH_PWR) || defined(_ARCH_PWR2) || defined(_POWER)
 #define SLJIT_CONFIG_PPC_32 1
-- 
2.28.1
```
