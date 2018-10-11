Changes to PCRE2 to adapt to the BDE Repo and Build Structure
=============================================================

This directory contains an almost-unmodified copy of pcre2 version 10.32.

However, a few minor build system changes were applied.

Build System Changes
====================

Created a `package/pcre2.cmake' file to build using CMake.

Created a `wscript` file to build using waf, based on instructions from the
`NON-AUTOTOOLS-BUILD` file.


