# NOTICE

Please do not use this package directly!  The facilities are available in the
BDE libraries though `bslalg_numericformatterutil` (and `bsl::to_chars`).

# INTRODUCTION

This is a packaged version of the open-source "ryu" project source code that
provides C facilities to convert IEEE-754 floating-point numbers to decimal
strings using shortest, fixed %f, and scientific %e formatting.

The original sources are from the https://github.com/ulfjack/ryu repository.

The library is pruned, as the parts of it that aren't fully portable or
finished have been removed.

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
| minimal    | A dual format (defined by C++17 `to_chars` function without a `format` argument) that uses decimal by default, but switches to the scientific if that is shorter, e.g., `105`, but `2E+05` for 200,000, and `2E-04` for 0.0002 |
| decimal    | The normal way humans usually write numbers e.g., `3.14159` or `42` |
| scientific | A significand followed by a signed decimal exponent expressed with at least 2 digits, e.g., `1.05E+02` for 105. |
| general    | A dual format (defined by the `'g'` format of `printf`) that also switches between the `'f'` and `'e'` formats, but uses an algorithm for switching that is different from the algorithm used by the minimal format. |
| `double`  | IEEE-754 double precision binary floating point, 64 bits in size |
| `float`   | IEEE-754 single precision binary floating point, 32 bits in size |
| precision | The number of fractional digits written |

`double` guarantees the storage of 16 significant decimal digits of a value,
but may require up to 17 significant digits to precisely represent its binary
value in decimal format.  Its decimal exponent's range is -324 to 308,
inclusive.

`float` guarantees the storage of 8 significant decimal digits of a value,
but may require up to 9 significant digits to precisely represent its binary
value in decimal format.  Its decimal exponent's range is -45 to 38, inclusive.

# WHY DID WE CHANGE/EXTEND Ryu?

BDE uses Ryu to convert floating point numbers to text almost the same way the
`to_chars` definition of ISO C++17, but there are differences.  ISO requires
us to choose between (the more human friendly) decimal and scientific format
based on the length of the final output.  Pure Ryu code does not support making
that decision.  Ryu originally does not support writing the shortest
round-tripping representation of a number in decimal format either.  It
supports the fixed format that requires a caller-supplied precision (number of
fractional digits to be written), however the shortest required precision for
round-tripping is not known to the callers.  Ryu's original scientific notation
format also differs slightly from the ISO C++ requirements, as C++ requires
the sign of the exponent to be always present, and the exponent to be written
using at least 2 digits (so 5 is 5E+00, 50 is 5E+01, etc.).

# VERSIONING

Please see `uor_version` in `package/ryu.pc.desc` for the version of the
original `ulfjack` code.  Since the original Ryu C implementation is a
research project and does not really have version numbers the `our_version` is
made up from the date and the short hash of the version of Ryu our code is
based on, and the version of local modifications: "YYYYMMDD.HHHHHHH.blpNN". The
NN in blpNN may be any number, increasing with any new release that is not an
update taken from the original Ryu code.

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

```cpp
int   blp_d2m_buffered_n(double f, char* result);
int   blp_f2m_buffered_n(float  f, char* result);

void  blp_d2m_buffered(double f, char* result);
void  blp_f2m_buffered(float  f, char* result);

int   blp_d2d_buffered_n(double f, char* result);
int   blp_f2d_buffered_n(float  f, char* result);

void  blp_d2d_buffered(double f, char* result);
void  blp_f2d_buffered(float  f, char* result);

int   blp_d2s_buffered_n(double f, char* result);
int   blp_f2s_buffered_n(float  f, char* result);

void  blp_d2s_buffered(double f, char* result);
void  blp_f2s_buffered(float  f, char* result);
```

The functions behave just like Ryu's own in regards to their interface/names:
* The `_buffered_n` functions just write the decimal form but no closing null
  character, and they return the number of characters written.
* The `_buffered` functions null terminate.
* Both the `_buffered_n`, and the `_buffered` functions require the caller to
  provide a buffer large enough to hold the longest conversion result:
  * `blp_d2m_buffered_n` needs `result` to have at least 24 bytes
  * `blp_d2m_buffered` needs at least 25 bytes
  * `blp_f2m_buffered_n` needs at least 15 bytes
  * `blp_f2m_buffered` needs at least 16 bytes
  * `blp_d2d_buffered_n` needs `result` to have at least 327 bytes
  * `blp_d2d_buffered` needs at least 328 bytes
  * `blp_f2d_buffered_n` needs at least 48 bytes
  * `blp_f2d_buffered` needs at least 49 bytes
  * `blp_d2s_buffered_n` needs `result` to have at least 24 bytes
  * `blp_d2s_buffered` needs at least 25 bytes
  * `blp_f2s_buffered_n` needs at least 15 bytes
  * `blp_f2s_buffered` needs at least 16 bytes

The functions have been implemented in the new `blp_d2cpp.c`, and
`blp_f2cpp.c` files that were based on `d2s.c`, and `f2s.c` respectively.

Common code used by `blp_d2cpp.c` and `blp_f2cpp.c` has been added in the
form of the new `blp_common.h` header.

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

For the functionality defined in `d2fixed.c` we need to use `cmake` to
configure the proper configuration macros to make the resulting code the most
efficient possible for the different platforms (depending on if `__int128` is
present etc.)

We may also add automatic configuration into `cmake` to chose between small
and large tables as offered by Ryu code, if we ever support optimization for
size UFIDs.

# BLOOMBERG RYU IN DETAIL

Our extensions are described in the order they are needed to be understood, not
as they appear in the files.

Only `blp_f2cpp.c` and its relation to `f2s.c` is explained in detail as the
`blp_d2cpp.c`/`d2s.c` pair is just a slightly longer incarnation of the same,
with additional code to handler more bits.

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
Bloomberg extensions declaring two sets of two functions for each formatting
supported.

#### "Minimal" Format

```c
int blp_d2m_buffered_n(double f, char* result);
int blp_f2m_buffered_n(float  f, char* result);
```

These functions write in the "minimal" format (defined by the ISO C++
`to_chars` without a format argument), without writing a null terminator
character at the end, and return the number of characters written.  The
`result` buffer is supplied by the caller and has to have at least 24
characters to write a `double`, and 15 to write a `float`.

```c
void blp_d2m_buffered(double f, char* result);
void blp_f2m_buffered(float  f, char* result);
```

These functions also write in "minimal" format, but in addition write a null
terminator character to the end of the buffer and return `void`.  The `result`
buffer is supplied by the caller as well, and has to have at least 25
characters to write a `double`, and 16 to write a `float`.

#### Decimal (a.k.a., Fixed) Format

```c
int blp_d2d_buffered_n(double f, char* result);
int blp_f2d_buffered_n(float  f, char* result);
```

These functions write in the decimal (also call fixed) format (defined by the
ISO C++ `printf` `'f'` format, but with dynamically determined precision for
round tripping) without writing a null terminator character at the end, and
return the number of characters written.  The `result` buffer is supplied by
the caller and has to have at least 327 characters to write a `double`, and
48 to write a `float`.

```c
void blp_d2d_buffered(double f, char* result);
void blp_f2d_buffered(float  f, char* result);
```

These functions also write in decimal format, but in addition write a null
terminator character to the end of the buffer and return `void`.  The `result`
buffer is supplied by the caller as well, and has to have at least 328
characters to write a `double`, and 49 to write a `float`.

#### Scientific Format

```c
int blp_d2s_buffered_n(double f, char* result);
int blp_f2s_buffered_n(float  f, char* result);
```

These functions write in the scientific format (defined by the ISO C++
`printf` `'e'` format, but with dynamically determined precision for
round tripping) without writing a null terminator character at the end, and
return the number of characters written.  The `result` buffer is supplied by
the caller and has to have at least 24 characters to write a `double`, and 15
to write a `float`.

```c
void blp_d2m_buffered(double f, char* result);
void blp_f2m_buffered(float  f, char* result);
```

These functions also write in scientific format, but in addition write a null
terminator character to the end of the buffer and return `void`.  The `result`
buffer is supplied by the caller as well, and has to have at least 25
characters to write a `double`, and 16 to write a `float`.

### Understanding The Details Using `blp_f2cpp.c`

The `blp_f2cpp.c` file was based on the Ryu original `f2s.c` file.  It is
suggested to open a favorite editor or viewer tool with both files
side-by-side.  If you decide to use a diff tool please prefer one that can do
in-line diffs and place manual diff alignments so you can compare more than one
function in `blp_f2cpp.c` to the `to_chars` function in `f2s.c`.

Use this description for understanding `blp_d2cpp.c` as well -- however, look
out for subtle differences as `double` is larger, and needs more decoding work.
A good in-line diffing `difftool` can make understanding the differences
between the `float` and `double` code much easier.

Please have the code in front of you, start from the end of the `blp_f2cpp.c`
file going backwards, and read the notes below as you encounter
functions/changes.  Keep the `f2s.c` file handy for comparisons.

**`blp_f2m_buffered`** this function is self explanatory, and very similar to
`f2s_buffered` in `f2s.c` except for changes in function names.

**`blp_f2m_buffered_n`**  is a modified copy of `f2s_buffered_n` from
`f2s.c`.  The differences are numerous, but easy to see with a good diff tool.
We moved where +/-0 is written because the `common.h` code of
`copy_special_str` (that the original code uses) writes zeros in scientific
notation, but this format requires decimal/fixed notation for zero.  We use a
different function instead of `copy_special_str` for infinity and NaN values
(`xsd_non_numerical_mapping`) so we can use the XSD 1.1 defined, so slightly
more portable `"+INF"`, `"-INF"`, and `"NaN"` strings.  We use signed positive
infinity here because our XML encoder traditionally used `"+INF"`.

Before you move on to understanding the rest of the code you need to understand
the meaning of data in the **`struct floating_decimal_32`** data structure.
It is near the beginning of the file.  Note that its `mantissa` member
represents itself.  In scientific notation the mantissa is in decimal form
with only one integer digit between 0-9.  The `mantissa` here is an integer
only.  The value represented by the data structure is
`mantissa + 10^exponent` e.g., m:123 e:2 is 12,300, but m:25 e:-2 is 0.25.

We also need to decide between the two possible formats, which is done before
calling `to_chars_m`, because we need to write certain integers precisely.
So determining the number of decimal digits in `v.mantissa` (`olength`) has
been moved into this function.  We determine which format is shorter using the
new `needs_decimal_notation_m` from `blp_common.h`.  It has an `_m` postfix
because the "general" format will a similar function to switch between the same
two formats.  That we plan to name `needs_decimal_notation_g`.

If the value is determined to be written in decimal (fixed) notation and it is
an integer value (see `is_integer` in this file), we delegate the precise
printing to `d2fixed_buffered_n` from `d2fixed.c`.  It is a `double`-based
function, but it works just fine for 'float', only a bit slower.

Finally, we call the new `to_chars_m` function to do the writing of the
`olength` `v.mantissa` decimal digits, in a format depending on the value of
the `needsDecimalNotation` argument.

The **`to_chars_m`** function has changed significantly from `f2s.c`
`to_chars`.  In addition to taking two more (precalculated) parameters,  it
also delegates the bulk of the work of writing the number to one of two
functions that represent the two different notations: `write_decimal` and
 `write_scientific`.

**`write_decimal`** is thoroughly commented.  This function, while using
 patterns from the original `to_chars` from `f2s.c`, is a new "algorithm"
developed in Bloomberg.  Clarity was preferred over speed.  It delegates
writing of the digits from `v.mantissa` to calls to the
`write_decimal_digits` function that is very similar to `write_mantisssa`
mentioned below, but does not write a decimal radix.  It is also made of the
bulk of `to_chars_` from `f2s.c`.  `write_decimal_digits` does not return a
value (number of digits written) as it always writes the number of decimal
digits in its `output` parameter, which should be `olength`.

The **`write_scientific`** function is quite compact because it delegates
most of its work to `write_mantissa`.  Our modifications to it include
writing exponent values according to ISO C++ requirements (always writing a
sign and writing at least two exponent digits).  So our scientific writing
of 5 will be  `"5E+00"`, but `f2s.c` writes `"5E0"`.

The **`write_mantissa`** function is where the bulk of the original
`to_chars` code ended up from `f2s.c`.  However our version does not count
the digits written, because it is actually easly calculated with a lot less
work at the end of the function.  The function is well documented in our code.

Finally we `#include "ryu/blp_common.h"` in addition to `common.h`, as well
as `#include "ryu/blp_ryu.h"`.  We still include `ryu.h` so we can call
`d2fixed_buffered_n`.

Notice that we have decided not to include the function that uses a `malloc`
returned buffer like `f2s.c` does.

`is_integer` and the `mantissa_countr_zero` function it calls are thoroughly
documented in the code.

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
2021.08.23 Added scientific and decimal notation support.
