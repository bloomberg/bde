// bdldfp_decimalimplutil.cpp                                         -*-C++-*-

#include <bdldfp_decimalimplutil.h>

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

///Implementation Notes
///--------------------
//
///The decNumber context/evironment is not thread-safe
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// The 'getDecNumberContext()' function (according to C99) should be returning
// a thread-local context/environment that was initialized as a copy of the
// environment of the creator thread.  That is near impossible to properly
// implement without extensive rewrite of the decNumber library and forcing
// decimal users to use a propriatery thread start/stop mechanism.  Even if we
// ignore the initialization-from-the-creator-thread requirement, due to
// limitations of operating systems we need to support (pre-Vista Microsoft
// Windows) we cannot use a __thread/declspec(thread) variable and be done with
// it.  (Pre-Vista Windows does not support thread local variables in DLLs that
// are loaded using 'LoadLibrary' - and that is exactly the use-case we must
// support on Windows.)
//
// Why can we get away with this?  Because our 'decNumber' context is a POD and
// it is statically initialized; furthermore the parts of it are either used
// read-only or write-only (in our current setup) because we do not provide
// user access to the environment.  And we can get away with the latter (at
// least in this early version) because we do not see a use case in our code
// base (it already uses floating-point) and neither can we imagine one in the
// near and mid-term future:
//
//: o Rounding mode setting is usually used to "patch-up" binary floating-point
//:   numbers that are known to store a decimal; or in scientific calculations
//:   that are run twice, with different rounding modes, and the difference in
//:   the results is used to determine the accuracy of the calculation.
//:   Neither of those two is appicable to decimals: decimals are already
//:   decimal, there is no need to round them back to decimal; and scientific
//:   (or statistical) calculations should not be done using decimal floating-
//:   point, since it is less accurate than binary.
//:
//: o The traps are basically useless as defined by C99 - they are required to
//:   raise a SIGFPE signal, which then gets absolutely no information about
//:   what went wrong.  Even the design of the 'decNumber' library suggests
//:   that there should be a better way: it does not raise SIGFPE, not even
//:   when traps are requested, unless a special function is called after every
//:   operation - by hand.
//:
//: o The status flags are the closest thing to be useful, however most code
//:   that uses floating-point (and we have seen it) does not use them.  Out of
//:   the available flags the "inexact" flag is the one that may be of use to
//:   a pedantic application; and which is not easy to detect with other
//:   methods.  Overflow, in our cases, is easily detected because our inputs
//:   are never infinity, so if the result is, we had an overflow.  Invalid
//:   operations are even easier, since they produce a NaN and NaNs propagate.
//:   A bit harder to detect underflow, but still possible.  In either case,
//:   with typical financial calculations, and typical financial input
//:   (numbers), and the supported ranges and significant digits of the
//:   IEEE-754 decimal floating arithmetic types all those exceptions are
//:   highly unlikely (except perhaps the "inexact" exception when it comes to
//:   corner cases, such as trying to express the US national debt in Japanese
//:   Yen).  Note that even one trillion dollars needs 12 significant digits
//:   only and a 64 bit decimal floating-point type provides 16; so we are able
//:   to accurately represent up to 100 trillion dollars with cents up to two
//:   digits!  The current US national debt (at the time of writing) requires
//:   14 significant digits to represent (cents are not used), meaning that we
//:   would need to worry if it has got a 100 times larger - and then we could
//:   just start doing national debt related calculations using 128 bits.
//
// To conclude: changing the 'decNumber' context/environment to be thread-local
// is non-trivial work with very little benefit while it holds inherent risks
// due to the need of modifying the 'decNumber' library code - which has no
// tests.
//
///Portable decimal floating-point literals: why and how
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Floating-point literals in C/C++ are binary.  (More precisely: they are the
// same radix as 'double', which is binary on all systems we use.  The C
// standardization committee plans to make it even more complicated by
// introducing a pragma that sets the radix of floating-point literals for
// each translation using seperately; let's hope they forget about it.)
// So writing:
//..
//  Decimal32 x(0.3);
//..
// will *not* result in a decimal 'x' storing *exactly* 0.3, what we wanted,
// because it is actually identical (in essential behavior) to this:
//..
//  static const double unnamed_constant = 0.3;
//  Decimal32 x(unnamed_constant);
//..
// and a binary floating-point format is unable to represent 0.3 precisely.  So
// we need decimal floating-point literals, and the C99 Decimal TR does
// add them; they are floating-point literals followed by the df, dd or dl
// suffix for 32 bit, 64 bit and 128 bit decimal types respectively.  However,
// our library has to support systems that do not implement the C Decimal TR
// and so they do not have those literals.  Furthermore where they are
// available, we want to use the literals and not some slower mechanism.  That
// means, unfortunately, macros.  Marcos that translate into decimal literals
// on platforms that support them, and to runtime parsing for others.  See the
// macros 'BDLDFP_DECIMALIMPLUTIL_DF', 'BDLDFP_DECIMALIMPLUTIL_DD' and
// '..._DL'.
//
// This brings up another issue: most of our platforms do not support decimal
// literals, therefore (unless we do something to address this) most of our
// programmers will *not* benefit from compile-time checking of their decimal
// literals; because we macro-magic them into strings and parse them runtime.
// So unless they build on an IBM machine, or have perfect test coverage, it is
// very hard for them to find out if their literals are wrong.  Therefore, for
// those platforms (that do not support decimal literals) we use a trick to
// force runtime checks anyway; we use the fact that decimal literals, without
// the special suffices look like binary floating-point literals.  So we don't
// just turn them into strings and pass them into our 'parseN' functions, but
// we also try to use them as binary floating-point numbers, and make sure we
// get a compile-time error if we cannot.  See the 'checkLiteral' functions and
// their implementation (in the header).  This is not a foolproof method of
// detecting all errors at compile time: it is still possible to write the name
// of a variable (of type 'double', or something that converts to 'double') and
// the code will compile (on 'decNumber' platforms) but blow up runtime.
// However the method does protect against the usual mistakes, such as trying
// to use an 'int' literal:
//..
//  BDLDFP_DECIMALIMPLUTIL_DD(42);   // ===> error, should be:
//  BDLDFP_DECIMALIMPLUTIL_DD(42.);  // with a decimal point
//  BDLDFP_DECIMALIMPLUTIL_DD(42e0); // or in scientific format
//..

#include <bdldfp_decimalplatform.h>
#include <bdldfp_uint128.h>

#include <bsls_assert.h>
#include <bsl_algorithm.h>
#include <bsl_bitset.h>
#include <bsl_cstring.h>
#include <bsl_string.h>

extern "C" {
#include <decSingle.h>  // Even in hardware modes, we need decNumber functions.
}

#if BDLDFP_DECIMALPLATFORM_C99_TR
#  ifndef  __STDC_WANT_DEC_FP__
#    error __STDC_WANT_DEC_FP__ must be defined on the command line!
     char die[-42];     // if '#error' unsupported
#  endif
#endif

namespace {

// Each of the 1000 combination of 3 digits, called declets, are mapped to a
// unique 10-bit field. Declets are used to represent the mantissa of a
// decimal floating  point number.
static unsigned short const declets[] = {
    00000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 0x008, 0x009,
    0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x017, 0x018, 0x019,
    0x020, 0x021, 0x022, 0x023, 0x024, 0x025, 0x026, 0x027, 0x028, 0x029,
    0x030, 0x031, 0x032, 0x033, 0x034, 0x035, 0x036, 0x037, 0x038, 0x039,
    0x040, 0x041, 0x042, 0x043, 0x044, 0x045, 0x046, 0x047, 0x048, 0x049,
    0x050, 0x051, 0x052, 0x053, 0x054, 0x055, 0x056, 0x057, 0x058, 0x059,
    0x060, 0x061, 0x062, 0x063, 0x064, 0x065, 0x066, 0x067, 0x068, 0x069,
    0x070, 0x071, 0x072, 0x073, 0x074, 0x075, 0x076, 0x077, 0x078, 0x079,
    0x00a, 0x00b, 0x02a, 0x02b, 0x04a, 0x04b, 0x06a, 0x06b, 0x04e, 0x04f,
    0x01a, 0x01b, 0x03a, 0x03b, 0x05a, 0x05b, 0x07a, 0x07b, 0x05e, 0x05f,
    0x080, 0x081, 0x082, 0x083, 0x084, 0x085, 0x086, 0x087, 0x088, 0x089,
    0x090, 0x091, 0x092, 0x093, 0x094, 0x095, 0x096, 0x097, 0x098, 0x099,
    0x0a0, 0x0a1, 0x0a2, 0x0a3, 0x0a4, 0x0a5, 0x0a6, 0x0a7, 0x0a8, 0x0a9,
    0x0b0, 0x0b1, 0x0b2, 0x0b3, 0x0b4, 0x0b5, 0x0b6, 0x0b7, 0x0b8, 0x0b9,
    0x0c0, 0x0c1, 0x0c2, 0x0c3, 0x0c4, 0x0c5, 0x0c6, 0x0c7, 0x0c8, 0x0c9,
    0x0d0, 0x0d1, 0x0d2, 0x0d3, 0x0d4, 0x0d5, 0x0d6, 0x0d7, 0x0d8, 0x0d9,
    0x0e0, 0x0e1, 0x0e2, 0x0e3, 0x0e4, 0x0e5, 0x0e6, 0x0e7, 0x0e8, 0x0e9,
    0x0f0, 0x0f1, 0x0f2, 0x0f3, 0x0f4, 0x0f5, 0x0f6, 0x0f7, 0x0f8, 0x0f9,
    0x08a, 0x08b, 0x0aa, 0x0ab, 0x0ca, 0x0cb, 0x0ea, 0x0eb, 0x0ce, 0x0cf,
    0x09a, 0x09b, 0x0ba, 0x0bb, 0x0da, 0x0db, 0x0fa, 0x0fb, 0x0de, 0x0df,
    0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x109,
    0x110, 0x111, 0x112, 0x113, 0x114, 0x115, 0x116, 0x117, 0x118, 0x119,
    0x120, 0x121, 0x122, 0x123, 0x124, 0x125, 0x126, 0x127, 0x128, 0x129,
    0x130, 0x131, 0x132, 0x133, 0x134, 0x135, 0x136, 0x137, 0x138, 0x139,
    0x140, 0x141, 0x142, 0x143, 0x144, 0x145, 0x146, 0x147, 0x148, 0x149,
    0x150, 0x151, 0x152, 0x153, 0x154, 0x155, 0x156, 0x157, 0x158, 0x159,
    0x160, 0x161, 0x162, 0x163, 0x164, 0x165, 0x166, 0x167, 0x168, 0x169,
    0x170, 0x171, 0x172, 0x173, 0x174, 0x175, 0x176, 0x177, 0x178, 0x179,
    0x10a, 0x10b, 0x12a, 0x12b, 0x14a, 0x14b, 0x16a, 0x16b, 0x14e, 0x14f,
    0x11a, 0x11b, 0x13a, 0x13b, 0x15a, 0x15b, 0x17a, 0x17b, 0x15e, 0x15f,
    0x180, 0x181, 0x182, 0x183, 0x184, 0x185, 0x186, 0x187, 0x188, 0x189,
    0x190, 0x191, 0x192, 0x193, 0x194, 0x195, 0x196, 0x197, 0x198, 0x199,
    0x1a0, 0x1a1, 0x1a2, 0x1a3, 0x1a4, 0x1a5, 0x1a6, 0x1a7, 0x1a8, 0x1a9,
    0x1b0, 0x1b1, 0x1b2, 0x1b3, 0x1b4, 0x1b5, 0x1b6, 0x1b7, 0x1b8, 0x1b9,
    0x1c0, 0x1c1, 0x1c2, 0x1c3, 0x1c4, 0x1c5, 0x1c6, 0x1c7, 0x1c8, 0x1c9,
    0x1d0, 0x1d1, 0x1d2, 0x1d3, 0x1d4, 0x1d5, 0x1d6, 0x1d7, 0x1d8, 0x1d9,
    0x1e0, 0x1e1, 0x1e2, 0x1e3, 0x1e4, 0x1e5, 0x1e6, 0x1e7, 0x1e8, 0x1e9,
    0x1f0, 0x1f1, 0x1f2, 0x1f3, 0x1f4, 0x1f5, 0x1f6, 0x1f7, 0x1f8, 0x1f9,
    0x18a, 0x18b, 0x1aa, 0x1ab, 0x1ca, 0x1cb, 0x1ea, 0x1eb, 0x1ce, 0x1cf,
    0x19a, 0x19b, 0x1ba, 0x1bb, 0x1da, 0x1db, 0x1fa, 0x1fb, 0x1de, 0x1df,
    0x200, 0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x208, 0x209,
    0x210, 0x211, 0x212, 0x213, 0x214, 0x215, 0x216, 0x217, 0x218, 0x219,
    0x220, 0x221, 0x222, 0x223, 0x224, 0x225, 0x226, 0x227, 0x228, 0x229,
    0x230, 0x231, 0x232, 0x233, 0x234, 0x235, 0x236, 0x237, 0x238, 0x239,
    0x240, 0x241, 0x242, 0x243, 0x244, 0x245, 0x246, 0x247, 0x248, 0x249,
    0x250, 0x251, 0x252, 0x253, 0x254, 0x255, 0x256, 0x257, 0x258, 0x259,
    0x260, 0x261, 0x262, 0x263, 0x264, 0x265, 0x266, 0x267, 0x268, 0x269,
    0x270, 0x271, 0x272, 0x273, 0x274, 0x275, 0x276, 0x277, 0x278, 0x279,
    0x20a, 0x20b, 0x22a, 0x22b, 0x24a, 0x24b, 0x26a, 0x26b, 0x24e, 0x24f,
    0x21a, 0x21b, 0x23a, 0x23b, 0x25a, 0x25b, 0x27a, 0x27b, 0x25e, 0x25f,
    0x280, 0x281, 0x282, 0x283, 0x284, 0x285, 0x286, 0x287, 0x288, 0x289,
    0x290, 0x291, 0x292, 0x293, 0x294, 0x295, 0x296, 0x297, 0x298, 0x299,
    0x2a0, 0x2a1, 0x2a2, 0x2a3, 0x2a4, 0x2a5, 0x2a6, 0x2a7, 0x2a8, 0x2a9,
    0x2b0, 0x2b1, 0x2b2, 0x2b3, 0x2b4, 0x2b5, 0x2b6, 0x2b7, 0x2b8, 0x2b9,
    0x2c0, 0x2c1, 0x2c2, 0x2c3, 0x2c4, 0x2c5, 0x2c6, 0x2c7, 0x2c8, 0x2c9,
    0x2d0, 0x2d1, 0x2d2, 0x2d3, 0x2d4, 0x2d5, 0x2d6, 0x2d7, 0x2d8, 0x2d9,
    0x2e0, 0x2e1, 0x2e2, 0x2e3, 0x2e4, 0x2e5, 0x2e6, 0x2e7, 0x2e8, 0x2e9,
    0x2f0, 0x2f1, 0x2f2, 0x2f3, 0x2f4, 0x2f5, 0x2f6, 0x2f7, 0x2f8, 0x2f9,
    0x28a, 0x28b, 0x2aa, 0x2ab, 0x2ca, 0x2cb, 0x2ea, 0x2eb, 0x2ce, 0x2cf,
    0x29a, 0x29b, 0x2ba, 0x2bb, 0x2da, 0x2db, 0x2fa, 0x2fb, 0x2de, 0x2df,
    0x300, 0x301, 0x302, 0x303, 0x304, 0x305, 0x306, 0x307, 0x308, 0x309,
    0x310, 0x311, 0x312, 0x313, 0x314, 0x315, 0x316, 0x317, 0x318, 0x319,
    0x320, 0x321, 0x322, 0x323, 0x324, 0x325, 0x326, 0x327, 0x328, 0x329,
    0x330, 0x331, 0x332, 0x333, 0x334, 0x335, 0x336, 0x337, 0x338, 0x339,
    0x340, 0x341, 0x342, 0x343, 0x344, 0x345, 0x346, 0x347, 0x348, 0x349,
    0x350, 0x351, 0x352, 0x353, 0x354, 0x355, 0x356, 0x357, 0x358, 0x359,
    0x360, 0x361, 0x362, 0x363, 0x364, 0x365, 0x366, 0x367, 0x368, 0x369,
    0x370, 0x371, 0x372, 0x373, 0x374, 0x375, 0x376, 0x377, 0x378, 0x379,
    0x30a, 0x30b, 0x32a, 0x32b, 0x34a, 0x34b, 0x36a, 0x36b, 0x34e, 0x34f,
    0x31a, 0x31b, 0x33a, 0x33b, 0x35a, 0x35b, 0x37a, 0x37b, 0x35e, 0x35f,
    0x380, 0x381, 0x382, 0x383, 0x384, 0x385, 0x386, 0x387, 0x388, 0x389,
    0x390, 0x391, 0x392, 0x393, 0x394, 0x395, 0x396, 0x397, 0x398, 0x399,
    0x3a0, 0x3a1, 0x3a2, 0x3a3, 0x3a4, 0x3a5, 0x3a6, 0x3a7, 0x3a8, 0x3a9,
    0x3b0, 0x3b1, 0x3b2, 0x3b3, 0x3b4, 0x3b5, 0x3b6, 0x3b7, 0x3b8, 0x3b9,
    0x3c0, 0x3c1, 0x3c2, 0x3c3, 0x3c4, 0x3c5, 0x3c6, 0x3c7, 0x3c8, 0x3c9,
    0x3d0, 0x3d1, 0x3d2, 0x3d3, 0x3d4, 0x3d5, 0x3d6, 0x3d7, 0x3d8, 0x3d9,
    0x3e0, 0x3e1, 0x3e2, 0x3e3, 0x3e4, 0x3e5, 0x3e6, 0x3e7, 0x3e8, 0x3e9,
    0x3f0, 0x3f1, 0x3f2, 0x3f3, 0x3f4, 0x3f5, 0x3f6, 0x3f7, 0x3f8, 0x3f9,
    0x38a, 0x38b, 0x3aa, 0x3ab, 0x3ca, 0x3cb, 0x3ea, 0x3eb, 0x3ce, 0x3cf,
    0x39a, 0x39b, 0x3ba, 0x3bb, 0x3da, 0x3db, 0x3fa, 0x3fb, 0x3de, 0x3df,
    0x00c, 0x00d, 0x10c, 0x10d, 0x20c, 0x20d, 0x30c, 0x30d, 0x02e, 0x02f,
    0x01c, 0x01d, 0x11c, 0x11d, 0x21c, 0x21d, 0x31c, 0x31d, 0x03e, 0x03f,
    0x02c, 0x02d, 0x12c, 0x12d, 0x22c, 0x22d, 0x32c, 0x32d, 0x12e, 0x12f,
    0x03c, 0x03d, 0x13c, 0x13d, 0x23c, 0x23d, 0x33c, 0x33d, 0x13e, 0x13f,
    0x04c, 0x04d, 0x14c, 0x14d, 0x24c, 0x24d, 0x34c, 0x34d, 0x22e, 0x22f,
    0x05c, 0x05d, 0x15c, 0x15d, 0x25c, 0x25d, 0x35c, 0x35d, 0x23e, 0x23f,
    0x06c, 0x06d, 0x16c, 0x16d, 0x26c, 0x26d, 0x36c, 0x36d, 0x32e, 0x32f,
    0x07c, 0x07d, 0x17c, 0x17d, 0x27c, 0x27d, 0x37c, 0x37d, 0x33e, 0x33f,
    0x00e, 0x00f, 0x10e, 0x10f, 0x20e, 0x20f, 0x30e, 0x30f, 0x06e, 0x06f,
    0x01e, 0x01f, 0x11e, 0x11f, 0x21e, 0x21f, 0x31e, 0x31f, 0x07e, 0x07f,
    0x08c, 0x08d, 0x18c, 0x18d, 0x28c, 0x28d, 0x38c, 0x38d, 0x0ae, 0x0af,
    0x09c, 0x09d, 0x19c, 0x19d, 0x29c, 0x29d, 0x39c, 0x39d, 0x0be, 0x0bf,
    0x0ac, 0x0ad, 0x1ac, 0x1ad, 0x2ac, 0x2ad, 0x3ac, 0x3ad, 0x1ae, 0x1af,
    0x0bc, 0x0bd, 0x1bc, 0x1bd, 0x2bc, 0x2bd, 0x3bc, 0x3bd, 0x1be, 0x1bf,
    0x0cc, 0x0cd, 0x1cc, 0x1cd, 0x2cc, 0x2cd, 0x3cc, 0x3cd, 0x2ae, 0x2af,
    0x0dc, 0x0dd, 0x1dc, 0x1dd, 0x2dc, 0x2dd, 0x3dc, 0x3dd, 0x2be, 0x2bf,
    0x0ec, 0x0ed, 0x1ec, 0x1ed, 0x2ec, 0x2ed, 0x3ec, 0x3ed, 0x3ae, 0x3af,
    0x0fc, 0x0fd, 0x1fc, 0x1fd, 0x2fc, 0x2fd, 0x3fc, 0x3fd, 0x3be, 0x3bf,
    0x08e, 0x08f, 0x18e, 0x18f, 0x28e, 0x28f, 0x38e, 0x38f, 0x0ee, 0x0ef,
    0x09e, 0x09f, 0x19e, 0x19f, 0x29e, 0x29f, 0x39e, 0x39f, 0x0fe, 0x0ff};

// Properties<32>, Properties<64>, and Properties<128> contain constants
// and member functions identifying key properties of these decimal types.
template <int Size>
struct Properties;

// Properties of the 32-bit decimal floating point type.
template <>
struct Properties<32>
{
    typedef unsigned long StorageType;
    typedef BloombergLP::bdldfp::DecimalImplUtil::ValueType32 ValueType;

    union ValueTypeRaw
    {
        StorageType bits;
        ValueType   value;
    };

    static const int         digits          = 7;
    static const int         combinationSize = 11;
    static const int         bias            = 101;
    static const int         maxExponent     = 90;
    static const StorageType signBit         = 0x80000000ul;
    static const long long   smallLimit      = 1000000ll;
    static const long long   mediumLimit     = 10000000ll;

    static inline unsigned int topDigit(unsigned long long value)
    {
        return static_cast<unsigned int>(value / smallLimit);
    }
    static inline unsigned long long lowDigits(unsigned long long value)
    {
        return value % smallLimit;
    }
    static inline StorageType setSignBit(StorageType value)
    {
        return value | signBit;
    }
    static inline void convert(ValueType *target, StorageType bits)
    {
        ValueTypeRaw v;
        v.bits = bits;
        *target = v.value;
    }
};

// Properties of the 64-bit decimal floating point type.
template <>
struct Properties<64>
{
    typedef unsigned long long StorageType;
    typedef BloombergLP::bdldfp::DecimalImplUtil::ValueType64 ValueType;

    union ValueTypeRaw
    {
        StorageType bits;
        ValueType   value;
    };

    static const int         digits          = 16;
    static const int         combinationSize = 13;
    static const int         bias            = 398;
    static const int         maxExponent     = 369;
    static const long long   smallLimit      = 1000000000000000ll;
    static const long long   mediumLimit     = 10000000000000000ll;
    static const StorageType signBit         = 0x8000000000000000ull;
    static const StorageType plusInfBits     = 0x7800000000000000ull;
    static const StorageType minusInfBits    = 0xF800000000000000ull;

    static inline unsigned int topDigit(unsigned long long value)
    {
        return static_cast<unsigned int>(value / smallLimit);
    }
    static inline unsigned long long lowDigits(unsigned long long value)
    {
        return value % smallLimit;
    }
    static inline StorageType setSignBit(StorageType value)
    {
        return value | signBit;
    }
    static inline void convert(ValueType *target, StorageType bits)
    {
        ValueTypeRaw v;
        v.bits = bits;
        *target = v.value;
    }
};

// Properties of the 128-bit decimal floating point type.
template <>
struct Properties<128>
{
    // Decimal floating point types has machine-dependent endianess.

    typedef BloombergLP::bdldfp::Uint128                       StorageType;
    typedef BloombergLP::bdldfp::DecimalImplUtil::ValueType128 ValueType;

    static const int digits          = 34;
    static const int combinationSize = 17;
    static const int bias            = 6176;
    static const int maxExponent     = 6111;

    static inline unsigned int topDigit(unsigned long long)
    {
        return 0;
    }
    static inline unsigned long long lowDigits(unsigned long long value)
    {
        return value;
    }
    static inline StorageType setSignBit(StorageType value)
    {
        StorageType signBit(0x8000000000000000ull, 0ull);
        return value | signBit;
    }
    static inline void convert(ValueType *target, StorageType bits)
    {
        bsl::memcpy(target, &bits, 16);
    }
};

// Create the binary-encoded declets of the mantissa, excluding the leading
// digit which is encoded in the combination field.
template <int Size>
static typename Properties<Size>::StorageType getDeclets(
                                                      unsigned long long value)
{
    typedef typename Properties<Size>::StorageType StorageType;

    unsigned int shift(0u);
    StorageType bits = StorageType();
    for (; value; value /= 1000ull, shift += 10u) {
        bits |= StorageType(declets[value % 1000ull]) << shift;
    }
    return bits;
}

// Create the binary-encoded combination field, which combines the exponent
// with the leading digit of the mantissa.
template <int Size>
static typename Properties<Size>::StorageType makeCombinationField(
                                                            unsigned int digit,
                                                            int          exp)
{
    // lower (size - 5) bits: the bits of the exp with the bias addded
    // top 5 bits: G0...G4
    // - top two bits of the exponent (E0E1)
    // - four bits of the digit (D0...D3)
    // G0G1G2G3G4  E0E1  D0D1D2D3 comment
    // 1 1 1 0 a   1 0   1 0 0 a  digit == 8 || digit == 9
    // 1 1 0 a b   0 a   1 0 0 b  digit == 8 || digit == 9
    // 1 0 a b c   1 0   0 a b c  digit <  8
    // 0 a b c d   0 a   0 b c d  digit <  8

    enum {
        size  = Properties<Size>::combinationSize,
        shift = size - 5,
        clear = (1ul << shift) - 1ul
    };
    unsigned long expo(exp + Properties<Size>::bias);

    typename Properties<Size>::StorageType exponent(expo & clear);
    exponent |= digit < 8u
        ? (((expo & (0x3ul << shift)) << 3u)
           | (digit << shift))
        : ((0x18ul << shift)
           | ((expo & (0x3ul << shift)) << 1u)
           | (digit & 0x1u) << shift);

    return exponent <<= (Size - size - 1);
}

// Given the mantissa, exponent, and sign, create the bits of the decimal
// floating point value.
template <int Size>
static typename Properties<Size>::StorageType
combineDecimalRaw(unsigned long long value,
                  int                exp,
                  bool               negative)
{
    typedef typename Properties<Size>::StorageType StorageType;
    StorageType exponent(makeCombinationField<Size>(
                                      Properties<Size>::topDigit(value), exp));
    if (!negative) {
        return exponent |
               getDeclets<Size>(Properties<Size>::lowDigits(value));  // RETURN
    }
    else {
        return Properties<Size>::setSignBit(exponent |
              getDeclets<Size>(Properties<Size>::lowDigits(value)));  // RETURN
    }
}

template <int Size>
static typename Properties<Size>::StorageType toDecimalRaw(
                                                   unsigned long long value,
                                                   int                exp,
                                                   bool               negative)
{
    return combineDecimalRaw<Size>(value, exp, negative);
}

template <int Size>
void toDecimalRaw(typename Properties<Size>::ValueType *target,
                  unsigned long long                    value)
{
    typename Properties<Size>::StorageType bits(::toDecimalRaw<Size>(
                                                             value, 0, false));
    Properties<Size>::convert(target, bits);
}

template<int Size>
void makeDecimalRaw(typename Properties<Size>::ValueType *target,
                    unsigned long long                    value,
                    int                                   exponent)
{
    typename Properties<Size>::StorageType bits(::toDecimalRaw<Size>(value,
                                                                     exponent,
                                                                     false));
    Properties<Size>::convert(target, bits);
}

template<int Size>
void makeDecimalRaw(typename Properties<Size>::ValueType *target,
                    signed long long                      value,
                    int                                   exponent)
{

    if (0 <= value) {
        typename Properties<Size>::StorageType bits(::toDecimalRaw<Size>(
                                                      value, exponent, false));
        Properties<Size>::convert(target, bits);
    }
    else if (value == std::numeric_limits<long long>::min()) {
        typename Properties<Size>::StorageType bits(
            ::toDecimalRaw<Size>(static_cast<unsigned long long>(
                  std::numeric_limits<long long>::max()) + 1, exponent, true));
        bits = Properties<Size>::setSignBit(bits);
        Properties<Size>::convert(target, bits);
    }
    else {
        typename Properties<Size>::StorageType bits(::toDecimalRaw<Size>(
                                                      -value, exponent, true));
        bits = Properties<Size>::setSignBit(bits);
        Properties<Size>::convert(target, bits);
    }
}

template<int Size>
void makeDecimalRaw(typename Properties<Size>::ValueType *target,
                    unsigned int                          value,
                    int                                   exponent)
{
    makeDecimalRaw<Size>(
                     target, static_cast<unsigned long long>(value), exponent);
}

template<int Size>
void makeDecimalRaw(typename Properties<Size>::ValueType *target,
                    signed int                            value,
                    int                                   exponent)
{
    makeDecimalRaw<Size>(
                       target, static_cast<signed long long>(value), exponent);
}

}  // close unnamed namespace

namespace BloombergLP {
namespace bdldfp {

namespace {

// Create the 64-bit decimal floating point value '+inf'.
static inline DecimalImplUtil::ValueType64 plusInf64()
{
    DecimalImplUtil::ValueType64 value;
    Properties<64>::convert(&value, Properties<64>::plusInfBits);
    return value;
}

// Create the 64-bit decimal floating point value '-inf'.
static inline DecimalImplUtil::ValueType64 minusInf64()
{
    DecimalImplUtil::ValueType64 value;
    Properties<64>::convert(&value, Properties<64>::minusInfBits);
    return value;
}

}  // close unnamed namespace

                 // Implementation based on the decNumber library.

decContext *DecimalImplUtil::getDecNumberContext()
    // Provides the decimal context required by the decNumber library functions
{
    static decContext context = { 0, 0, 0, DEC_ROUND_HALF_EVEN, 0, 0, 0 };
    return &context;
}


DecimalImplUtil::ValueType32 DecimalImplUtil::parse32(const char *input)
{
    BSLS_ASSERT(input != 0);

    ValueType32 out;
#if BDLDFP_DECIMALPLATFORM_C99_TR
    // TBD TODO - scanf is locale dependent!!!
    int parsed = sscanf(input, "%Hf", &out);
    (void) parsed;
    BSLS_ASSERT(parsed == 1);
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    void *ptr = decSingleFromString(&out, input, getDecNumberContext());
    (void) ptr;
    BSLS_ASSERT(ptr != 0);
#endif
    return out;
}

DecimalImplUtil::ValueType64 DecimalImplUtil::parse64(const char *input)
{
    BSLS_ASSERT(input != 0);

    ValueType64 out;
#if BDLDFP_DECIMALPLATFORM_C99_TR
    // TBD TODO - scanf is locale dependent!!!
    int parsed = sscanf(input, "%Df", &out);
    (void) parsed;
    BSLS_ASSERT(parsed == 1);
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    void *ptr = decDoubleFromString(&out, input, getDecNumberContext());
    (void) ptr;
    BSLS_ASSERT(ptr != 0);
#endif
    return out;
}

DecimalImplUtil::ValueType128 DecimalImplUtil::parse128(const char *input)
{
    BSLS_ASSERT(input != 0);

    ValueType128 out;
#if BDLDFP_DECIMALPLATFORM_C99_TR
    // TBD TODO - scanf is locale dependent!!!
    int parsed = sscanf(input, "%DDf", &out);
    (void) parsed;
    BSLS_ASSERT(parsed == 1);
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    void *ptr = decQuadFromString(&out, input, getDecNumberContext());
    (void) ptr;
    BSLS_ASSERT(ptr != 0);
#endif
    return out;
}


DecimalImplUtil::ValueType32 DecimalImplUtil::convertToDecimal32(
                                                      const ValueType64& input)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return static_cast<ValueType32>(input);
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    ValueType32 other;
    decSingleFromWider(&other, &input, getDecNumberContext());
    return other;
#endif
}

DecimalImplUtil::ValueType64 DecimalImplUtil::convertToDecimal64(
                                                      const ValueType32& input)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return static_cast<ValueType64>(input);
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    ValueType64 other;
    decSingleToWider(&input, &other);
    return other;
#endif
}

DecimalImplUtil::ValueType64 DecimalImplUtil::convertToDecimal64(
                                                     const ValueType128& input)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return static_cast<ValueType64>(input);
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    ValueType64 other;
    decDoubleFromWider(&other, &input, getDecNumberContext());
    return other;
#endif
}

DecimalImplUtil::ValueType128 DecimalImplUtil::convertToDecimal128(
                                                      const ValueType32& input)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return static_cast<ValueType128>(input);
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return convertToDecimal128(convertToDecimal64(input));
#endif
}

DecimalImplUtil::ValueType128 DecimalImplUtil::convertToDecimal128(
                                                      const ValueType64& input)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return static_cast<ValueType128>(input);
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    ValueType128 other;
    decDoubleToWider(&input, &other);
    return other;
#endif
}

DecimalImplUtil::ValueType32 DecimalImplUtil::makeDecimalRaw32(int mantissa,
                                                               int exponent)
{
    BSLS_ASSERT(-101     <= exponent);
    BSLS_ASSERT(exponent <= 90);
    BSLS_ASSERT(bsl::max(mantissa, -mantissa) <= 9999999);

    // TODO: no '__d32_insert_biased_exponent' function.
    ValueType32 valuetype32;
    makeDecimalRaw<32>(&valuetype32, mantissa, exponent);
    return valuetype32;
}

DecimalImplUtil::ValueType64 DecimalImplUtil::makeDecimalRaw64(
                                                         unsigned int mantissa,
                                                         int          exponent)
{
    BSLS_ASSERT(-398     <= exponent);
    BSLS_ASSERT(exponent <= 369);

#if BDLDFP_DECIMALPLATFORM_C99_TR
    _Decimal64 value = mantissa;
    return __d64_insert_biased_exponent(value, exponent + 398);
#else
    ValueType64 valuetype64;
    makeDecimalRaw<64>(&valuetype64, mantissa, exponent);

    return valuetype64;
#endif
}

DecimalImplUtil::ValueType64 DecimalImplUtil::makeDecimalRaw64(int mantissa,
                                                               int exponent)
{
    BSLS_ASSERT(-398     <= exponent);
    BSLS_ASSERT(exponent <= 369);

#if BDLDFP_DECIMALPLATFORM_C99_TR
    _Decimal64 value = mantissa;
    return __d64_insert_biased_exponent(value, exponent + 398);
#else
    ValueType64 valuetype64;
    makeDecimalRaw<64>(&valuetype64, mantissa, exponent);

    return valuetype64;
#endif
}

DecimalImplUtil::ValueType64 DecimalImplUtil::makeDecimalRaw64(
                                                   unsigned long long mantissa,
                                                   int                exponent)
{
    BSLS_ASSERT(-398     <= exponent);
    BSLS_ASSERT(exponent <= 369);
    BSLS_ASSERT(mantissa <= 9999999999999999LL);\

#if BDLDFP_DECIMALPLATFORM_C99_TR
    _Decimal64 value = mantissa;
    return __d64_insert_biased_exponent(value, exponent + 398);
#else
    ValueType64 valuetype64;
    makeDecimalRaw<64>(&valuetype64, mantissa, exponent);

    return valuetype64;
#endif
}

DecimalImplUtil::ValueType64 DecimalImplUtil::makeDecimalRaw64(
                                                            long long mantissa,
                                                            int       exponent)
{
    BSLS_ASSERT(-398     <= exponent);
    BSLS_ASSERT(exponent <= 369);
    BSLS_ASSERT(std::max(mantissa, -mantissa) <= 9999999999999999LL);

#if BDLDFP_DECIMALPLATFORM_C99_TR
    _Decimal64 value = mantissa;
    return __d64_insert_biased_exponent(value, exponent + 398);
#else
    ValueType64 valuetype64;
    makeDecimalRaw<64>(&valuetype64, mantissa, exponent);

    return valuetype64;
#endif
}

DecimalImplUtil::ValueType128 DecimalImplUtil::makeDecimalRaw128(
                                                         unsigned int mantissa,
                                                         int          exponent)
{
    BSLS_ASSERT(-6176    <= exponent);
    BSLS_ASSERT(exponent <= 6111);

#if BDLDFP_DECIMALPLATFORM_C99_TR
    _Decimal128 value = mantissa;
    return __d128_insert_biased_exponent(value, exponent + 6176);
#else
    ValueType128 valuetype128;
    makeDecimalRaw<128>(&valuetype128, mantissa, exponent);
    return valuetype128;
#endif
}

DecimalImplUtil::ValueType128 DecimalImplUtil::makeDecimalRaw128(int mantissa,
                                                                 int exponent)
{
    BSLS_ASSERT(-6176    <= exponent);
    BSLS_ASSERT(exponent <= 6111);

#if BDLDFP_DECIMALPLATFORM_C99_TR
    _Decimal128 value = mantissa;
    return __d128_insert_biased_exponent(value, exponent + 6176);
#else
    ValueType128 valuetype128;
    makeDecimalRaw<128>(&valuetype128, mantissa, exponent);
    return valuetype128;
#endif
}

DecimalImplUtil::ValueType128 DecimalImplUtil::makeDecimalRaw128(
                                                   unsigned long long mantissa,
                                                   int                exponent)
{
    BSLS_ASSERT(-6176    <= exponent);
    BSLS_ASSERT(exponent <= 6111);

#if BDLDFP_DECIMALPLATFORM_C99_TR
    _Decimal128 value = mantissa;
    return __d128_insert_biased_exponent(value, exponent + 6176);
#else
    ValueType128 valuetype128;
    makeDecimalRaw<128>(&valuetype128, mantissa, exponent);
    return valuetype128;
#endif
}

DecimalImplUtil::ValueType128 DecimalImplUtil::makeDecimalRaw128(
                                                           long long  mantissa,
                                                           int        exponent)
{
    BSLS_ASSERT(-6176    <= exponent);
    BSLS_ASSERT(exponent <= 6111);

#if BDLDFP_DECIMALPLATFORM_C99_TR
    _Decimal128 value = mantissa;
    return __d128_insert_biased_exponent(value, exponent + 6176);
#else
    ValueType128 valuetype128;
    makeDecimalRaw<128>(&valuetype128, mantissa, exponent);
    return valuetype128;
#endif
}

DecimalImplUtil::ValueType64 DecimalImplUtil::makeDecimal64(
                                                   unsigned long long mantissa,
                                                   int                exponent)
{
    if ((-Properties<64>::bias <= exponent) &&
        (exponent <= Properties<64>::maxExponent) &&
        (mantissa <
               static_cast<unsigned long long>(Properties<64>::mediumLimit))) {

        // 'mantissa' and 'exponent' are in range of 64-bit decimal floating
        // point.

        return makeDecimalRaw64(mantissa, exponent);                  // RETURN
    }
    else {
        if (exponent >= Properties<64>::maxExponent + Properties<64>::digits) {

            // 'exponent' too high.

            if (mantissa != 0) {
                return plusInf64();                                   // RETURN
            }
            else {

                // Make a '0' with the highest exponent possible.

                return makeDecimalRaw64(
                                    0, Properties<64>::maxExponent);  // RETURN
            }
        }

        // Note that static_cast<int> is needed to prevent the RHS of the <=
        // comparison from promoting to a signed int. '3 * sizeof(long long)'
        // is at least the number of digits in the longest representable
        // 'long long'.

        else if (exponent <= -Properties<64>::bias -
                                     3 * static_cast<int>(sizeof(long long))) {

            // 'exponent' too low.

            return makeDecimalRaw64(0, -Properties<64>::bias);        // RETURN
        }
        else {

            // Precision too high.

            return convertToDecimal64(
                             makeDecimalRaw128(mantissa, exponent));  // RETURN
        }
    }

    ValueType64 valuetype64;
    makeDecimalRaw<64>(&valuetype64, mantissa, exponent);
    return valuetype64;
}

DecimalImplUtil::ValueType64 DecimalImplUtil::makeDecimal64(long long mantissa,
                                                            int       exponent)
{
    if (((-Properties<64>::bias) <= exponent) &&
        (exponent <= Properties<64>::maxExponent) &&
        ((-Properties<64>::mediumLimit) < mantissa) &&
        (mantissa < Properties<64>::mediumLimit)) {

        // 'mantissa' and 'exponent' are in range of 64-bit decimal floating
        // point.

        return makeDecimalRaw64(mantissa, exponent);                  // RETURN
    }
    else {
        if (exponent >= Properties<64>::maxExponent + Properties<64>::digits) {

            // 'exponent' too high.

            if (mantissa > 0) {
                return plusInf64();                                   // RETURN
            }
            else if (mantissa < 0) {
                return minusInf64();                                  // RETURN
            }
            else {

                // Make a '0' with the highest exponent possible.

                return makeDecimalRaw64(
                                    0, Properties<64>::maxExponent);  // RETURN
            }
        }
        else if (exponent <= -Properties<64>::bias -
                                     3 * static_cast<int>(sizeof(long long))) {

            // 'exponent' too low.

            if (mantissa >= 0) {
                return makeDecimalRaw64(0, -Properties<64>::bias);    // RETURN
            }
            else {

                // Create and return the decimal floating point value '-0'.

                Properties<64>::StorageType returnBits =
                         combineDecimalRaw<64>(0, -Properties<64>::bias, true);
                ValueType64 returnValue;
                Properties<64>::convert(&returnValue, returnBits);
                return returnValue;                                   // RETURN
            }
        }
        else {

            // Precision too high.

            return convertToDecimal64(
                             makeDecimalRaw128(mantissa, exponent));  // RETURN
        }
    }

    ValueType64 valuetype64;
    makeDecimalRaw<64>(&valuetype64, mantissa, exponent);
    return valuetype64;
}

DecimalImplUtil::ValueType64 DecimalImplUtil::makeDecimal64(
                                                         unsigned int mantissa,
                                                         int          exponent)
{
    if ((-Properties<64>::bias <= exponent) &&
        (exponent <= Properties<64>::maxExponent)) {
        return makeDecimalRaw64(mantissa, exponent);                  // RETURN
    }
    else {
        if (exponent >= Properties<64>::maxExponent + Properties<64>::digits) {
            if (mantissa != 0) {
                return plusInf64();                                   // RETURN
            }
            else {
                return makeDecimalRaw64(
                    0, Properties<64>::maxExponent);                  // RETURN
            }
        }
        else if (exponent <= -Properties<64>::bias - Properties<64>::digits) {
            return makeDecimalRaw64(0, -Properties<64>::bias);        // RETURN
        }
        else {
            return convertToDecimal64(
                             makeDecimalRaw128(mantissa, exponent));  // RETURN
        }
    }

    ValueType64 valuetype64;
    makeDecimalRaw<64>(&valuetype64, mantissa, exponent);
    return valuetype64;
}

DecimalImplUtil::ValueType64 DecimalImplUtil::makeDecimal64(int mantissa,
                                                            int exponent)
{
    if ((-Properties<64>::bias <= exponent) &&
        (exponent <= Properties<64>::maxExponent)) {
        return makeDecimalRaw64(mantissa, exponent);                  // RETURN
    }
    else {
        if (exponent >= Properties<64>::maxExponent + Properties<64>::digits) {
            if (mantissa > 0) {
                return plusInf64();                                   // RETURN
            }
            else if (mantissa < 0) {
                return minusInf64();                                  // RETURN
            }
            else {
                return makeDecimalRaw64(0,
                                       Properties<64>::maxExponent);  // RETURN
            }
        }
        else if (exponent <= -Properties<64>::bias - Properties<64>::digits) {
            return makeDecimalRaw64(0, -Properties<64>::bias);        // RETURN
        }
        else {
            return convertToDecimal64(
                             makeDecimalRaw128(mantissa, exponent));  // RETURN
        }
    }

    ValueType64 valuetype64;
    makeDecimalRaw<64>(&valuetype64, mantissa, exponent);
    return valuetype64;
}

bool DecimalImplUtil::equals(DecimalImplUtil::ValueType128 lhs,
                             DecimalImplUtil::ValueType128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs == rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decQuad result;
    decQuadCompare(&result, &lhs, &rhs, getDecNumberContext());
    return decQuadIsZero(&result);
#endif
}

bool DecimalImplUtil::equals(DecimalImplUtil::ValueType64 lhs,
                             DecimalImplUtil::ValueType64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs == rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    decDouble result;
    decDoubleCompare(&result, &lhs, &rhs, getDecNumberContext());
    return decDoubleIsZero(&result);
#endif
}

bool DecimalImplUtil::equals(DecimalImplUtil::ValueType32 lhs,
                             DecimalImplUtil::ValueType32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs == rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return equals(convertToDecimal64(lhs), convertToDecimal64(rhs));
#endif
}


bool DecimalImplUtil::equals(DecimalImplUtil::ValueType32 lhs,
                             DecimalImplUtil::ValueType64 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs == rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return equals(convertToDecimal64(lhs), rhs);
#endif
}

bool DecimalImplUtil::equals(DecimalImplUtil::ValueType64 lhs,
                             DecimalImplUtil::ValueType32 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs == rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return equals(lhs, convertToDecimal64(rhs));
#endif
}

bool DecimalImplUtil::equals(DecimalImplUtil::ValueType32  lhs,
                             DecimalImplUtil::ValueType128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs == rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return equals(convertToDecimal128(lhs), rhs);
#endif
}

bool DecimalImplUtil::equals(DecimalImplUtil::ValueType128 lhs,
                             DecimalImplUtil::ValueType32  rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs == rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return equals(lhs, convertToDecimal128(rhs));
#endif
}

bool DecimalImplUtil::equals(DecimalImplUtil::ValueType64  lhs,
                             DecimalImplUtil::ValueType128 rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs == rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return equals(convertToDecimal128(lhs), rhs);
#endif
}

bool DecimalImplUtil::equals(DecimalImplUtil::ValueType128 lhs,
                             DecimalImplUtil::ValueType64  rhs)
{
#if BDLDFP_DECIMALPLATFORM_C99_TR
    return lhs == rhs;
#elif BDLDFP_DECIMALPLATFORM_DECNUMBER
    return equals(lhs, convertToDecimal128(rhs));
#endif
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
