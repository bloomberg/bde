# NOTICE

Please do not use this package directly!  The facilities are available in the
BDE libraries though `bslalg_numericformatterutil` (and `bsl::to_chars`).

# INTRODUCTION

This is a packaged version of the open-source "ryu" project source code that
provides C facilities to convert IEEE-754 floating-point numbers to decimal
strings using shortest, fixed %f, and scientific %e formatting.

The original sources are from the https://github.com/ulfjack/ryu repository.

The library is pruned, as parts that aren't fully portable have been removed.

Ryu is both an algorithm and a fast library (the original incarnation written
in C) that converts between textual decimal and IEEE-754 binary single or
double precision floating point representations of numbers.  In other words it
converts to decimal text from `float` and `double`.  It also supports
determining the smallest number of significant decimal digits to use so that
when the number is converted back to binary we get back the same number.  This
property is called shortest round-tripping representation.

Note that the 'ryu' source code was open sourced as a research experiment,
without infrastructure for packaging.  I.e., the open source repository just
provides a collection of C files, which we have provided packaging for as well
as a couple important extensions.

# TERMINOLOGY AND CONSTANTS

Good to know before looking into details.

| Term       | Definition |
| ---------- | ---------- |
| decimal    | The normal way humans usually write numbers e.g., `3.14159` or `42` |
| scientific | A significand followed by a signed decimal exponent expressed with at least 2 digits, e.g., `1.05E+02` for 105. |
| general    | A dual format that uses decimal by default, but switches to the scientific if that is shorter, e.g., `105`, but `2E+05` for 200,000, and `2E-04` for 0.0002 |
| `double`  | IEEE-754 double precision binary floating point, 64 bits in size |
| `float`   | IEEE-754 single precision binary floating point, 32 bits in size |

`double` guarantees the storage of 16 significant decimal digits of a value,
but may require up to 17 significant digits to precisely represent its binary
value in decimal format.  Its decimal exponent's range is -324 to 308,
inclusive.

`float` guarantees the storage of 8 significant decimal digits of a value,
but may require up to 9 significant digits to precisely represent its binary
value in decimal format.  Its decimal exponent's range is -45 to 38, inclusive.

# WHY DID WE CHANGE/EXTEND Ryu?

BDE uses Ryu to convert floating point numbers to text almost the same way the
`to_chars` definition of ISO C++ 17 (we don't implement a corner case with
very large integers).  That definition requires us to choose between scientific
and decimal (the more human friendly) format based on the length of the final
output.  Ryu does not support making that decision.  Ryu originally does not
support writing the shortest round-tripping representation of a number in
decimal format.  It supports only the so-called fixed format that requires a
caller-supplied precision (number of fractional digits), however the shortest
precision for round-tripping is not known to the callers.  Ryu's original
scientific notation also differs from the ISO C++ requirements as C++ requires
the sign of the exponent to be always present, and the exponent to be written
using at least 2 digits (so 5 is 5E+00, 50 is 5E+01, etc.).

# VERSIONING

Please see `uor_version` in `package/ryu.pc.desc` for the version of the
original `ulfjack` code.  Since the original Ryu C implementation is a
research project and does not really have version numbers the `our_version` is
made up from the date and the short hash of the version of Ryu our code is
based on: "YYYY.MMDD.HHHHHHH".

# UPDATING

We do not plan to update the code unless Ryu is updated with a bug fix, or it
has a major overhaul that would make it difficult follow its updates.  We have
made no changes to the original Ryu files in order to make keeping them
up-to-date easier.  Because all our Ryu extensions are based on original Ryu
code a single update to original Ryu may require several updates in our copy.
The BLOOMBERG RYU IN DETAIL section describes *all* the extensions, and what
original code those extensions were based on.  That should make applying fixes,
or recreating/understanding the extensions easier.

Don't forget to update `uor_version` in `package/ryu.pc.desc` so it reflects
the new "base version" of Ryu during updating.  See VERSIONING above.

Don't forget to extend the `Bloomberg LP CHANGELOG` in `blp_f2g.c` and
`blp_d2g.c` so that it shows that "upstream" fixes have been applied.
Remember to mention what changes were *not* applied, and why.  It is quite
alright not to apply changes that are not bug fixes and would make our code
non-portable, or such changes that would require a major rewrite of our code.
Ryu was well tested by its author as well as by Microsoft when they decided to
base their C++17 `std::to_chars` implementation on it.  Therefore we expect
runtime performance related updates that are mostly unimportant for our use
case, and not bug fixes.

A CHANGELOG section also exists at the end of this document where we will list
further updates in less detail.

# THE SHORT STORY

See BLOOMBERG RYU IN DETAIL for code level details.

## WHAT WAS ADDED

We extended Ryu with the following "public" functions, added in the new
`blp_ryu.h` header:

int   blp_d2g_buffered_n(double f, char* result);
int   blp_f2g_buffered_n(float  f, char* result);

void  blp_d2g_buffered(double f, char* result);
void  blp_f2g_buffered(float  f, char* result);

The functions behave just like Ryu's own in regards to their interface/names:
* The `_buffered_n` functions just write the decimal form but no closing null
  character, and they return the number of characters written.
* The `_buffered` functions null terminate.
* Both the `_buffered_n`, and the `_buffered` functions require the caller to
  provide a buffer large enough to hold the longest conversion result:
  * `d2g_buffered_n` needs `result` to have at least 24 bytes
  * `d2g_buffered` needs at least 25 bytes
  * `f2g_buffered_n` needs at least 15 bytes
  * `f2g_buffered` needs at least 16 bytes

The functions have been implemented in the new `blp_d2g.c`, and `blp_f2g.c`
files that were based on copies of `d2s.c`, and `f2s.c`, respectively.

Common code used by `blp_d2g.c` and `blp_f2g.c` has been added in the form of
the new `blp_common.h` header.

# WHAT WAS PRUNED

The original Ryu files `generic_128.h`, `ryu_generic_128.h`, and
`generic_128.c` have been removed as they are not portable to several of the
compilers we need to support (they work on gcc/clang only due to using
`__int128t`).

The original Ryu files `ryu_parse.h`, `s2f.c`, and `s2d.c` have been removed
as they are not portable to several of the platforms we need to support (32 bit
Windows and Solaris) due to the use of complier intrinsics that do not exist on
those platforms (resulting in unresolved externals on use).  The
string-to-binary conversion was added to Ryu as testing needed something that
can read back the text Ryu writes for NaN and Infinity values.

Adding the above files should be considered if and when
* the author considers them stable
* they get support at the same level of enthusiasm as the rest of the
  algorithms
* they either become portable to all our supported platforms or necessary on
  the already supported ones
* we have a business reason for adding them.

# WHAT IS LEFT TO DO

If the functionality defined in `d2fixed.c` is ever to be used we need to use
`cmake` to configure the proper configuration macros to make the resulting
code most efficient.

# BLOOMBERG RYU IN DETAIL

Our extensions are described in the order they are needed to be understood, not
as they appear in the files.

Only `d2g.c` and its relation to `d2s.c` is explained in detail as the
`f2g.c`/`f2s.c` pair is just a slightly simpler incarnation of the same.

## THE UNCHANGED PARTS

1. From https://github.com/ulfjack/ryu/ we have used the following files:
    1. "Legal" files from the root folder:
        1.  `CONTRIBUTORS`
        2.  `LICENSE-Apache2`
        3.  `LICENSE-Boost`

    2. Source files from the `ryu` folder:
        1. `ryu/common.h`
        2. `ryu/d2s.c`
        3. `ryu/d2s_full_table.h`
        4. `ryu/d2s_intrinsics.h`
        5. `ryu/d2s_small_table.h`
        6. `ryu/digit_table.h`
        7. `ryu/f2s.c`
        8. `ryu/f2s_full_table.h`
        9. `ryu/f2s_intrinsics.h`
       10. `ryu/ryu.h`


## THE BLOOMBERG EXTENSIONS

The Bloomberg extensions reside in files with the `blp_` prefix.  Every public
symbol we have added is a function, and each has the `blp_` prefix as well.
Files that we added with totally new content have BDE-like formatting as well
as our Bloomberg Open Source copying with the Apache 2 License.  The two `.c`
implementation files that were largely based on the original Ryu code for
scientific notation in shortest round-trip format retain their original
copyright and license messages with an added `Bloomberg LP CHANGELOG`
section.

### `bde_ryu.h`

Please see the file for documentation.  It is the public interface for the
Bloomberg extensions declaring two sets of two functions:
```c
int blp_d2g_buffered_n(double f, char* result);
int blp_f2g_buffered_n(float  f, char* result);
```
These write without writing a null terminator character and return the number
of characters written.  The `result` buffer is supplied by the caller and has
to have at least 24 characters to write a `double`, and 15 to write a
`float`.

```c
void blp_d2g_buffered(double f, char* result);
void blp_f2g_buffered(float  f, char* result);
```
These also write the null terminator character to the end and do not return
anything.  The `result` buffer is supplied by the caller and has to have at
least 25 characters to write a `double`, and 16 to write a `float`.

### Understanding The Details Using `blp_f2g.c`

The `blp_f2g.c` file was created from the Ryu original `f2s.c` file.  It is
suggested to open a favorite editor or viewer tool with both files
side-by-side.  If you decide to use a diff tool please prefer one that can do
in-line diffs and place manual diff alignments so you can compare more than one
function in `blp_f2g.c` to the `to_chars` function in `f2s.c`.

Use this description for understanding `blp_d2g.c` as well -- however, look out
for subtle differences as `double` is a larger, and needs more decoding work.
A good in-line diffing `difftool` can make understanding the differences much
easier.

Please have the code in front of you, start from the end of the `blp_f2g.c`
file going backwards, and read the notes below as you encounter
functions/changes.  Keep the `f2s.c` file handy for comparisons.

**`blp_f2g_buffered`** this function is self explanatory, and essentially
identical to `f2s_buffered` in `f2s.c` except for the changes in function
names.

**`blp_f2g_buffered_n`**  is a modified copy of `f2s_buffered_n` from
`f2s.c`.  The differences are small and easy to see.  We moved where +/-0 is
written because the `common.h` code of `copy_special_str` (that the original
code uses) writes zeros in scientific notation, but ISO C++ general format
requires decimal.

Before you move on to understanding the rest of the code you need to understand
the meaning of data in the **`struct floating_decimal_32`** data structure.
It is near the beginning of the file.  Note that its `mantissa` member
represents itself.  In scientific notation the mantissa is in decimal form
with only one integer digit between 0-9.  The `mantissa` here is an integer
only.  The value represented by the data structure is
`mantissa + 10^exponent` e.g., m:123 e:2 is 12,300, but m:25 e:-2 is 0.25.

The **`to_chars`** function has changed significantly from `f2s.c`.  It
delegates the bulk of the work of writing the number to one of two functions
that represent the two different notations.


**`needs_decimal_notation` in `blp_common.h`** is a function that has been
first independently developed (identical logic), but Microsoft's Apache 2.0
licensed version was incorporated into it for the nice explanatory comments.

**Back in `blp_f2g.c` `write_decimal`** is thoroughly commented.  This
function, while using patterns from the original `to_chars` from `f2s.c`, is a
new "algorithm" developed in Bloomberg.  Clarity was preferred over speed.

The **`write_scientific`** function is quite compact because it delegates
most of its work to `write_digits`.  Our modifications to it include ensuring
that it writes exponent values according to ISO C++ requirements, so writing a
sign also for non-negative exponents and writing at least two exponent digits.
So our scientific writing would write 5 as `"5E+00"`, but `f2s.c` writes
`"5E0"`.

The **`write_digits`** function is where the bulk of the original `to_chars`
code ended up from `f2s.c`.  However our version has two write modes:
`DigitsOnly` and `Mantissa`.  These modes are represented by a shared `enum`
in `blp_common.h` with the name `WriteDigitsMode`.  The function is well
documented in our code.

Finally we `#include "ryu/blp_common.h"` in addition to `common.h`, and well
as `#include "ryu/blp_ryu.h"` *instead* of `ryu.h`.

Notice that we have decided not to include the function that uses a `malloc`
returned buffer.

When you compare `blp_f2g.c` with `blp_d2g.c` you will notice that it has a
special function to detect small integers.  Additional code in
`blp_d2g_buffered` decodes those small integers.  Because `double` has 52
bits in its significand `blp_d2g.c` uses a 64 bit type for `mantissa` and
`output` and its `write_digits` function has an additional section that deals
with printing the top 8 digits of very long values.  Because the decimal
exponent of `double` values can go up to 3 digits `blp_d2g.c` also has extra
code for that.

# CHANGELOG

2021.07.16 First version.  See BLOOMBERG RYU IN DETAIL.
