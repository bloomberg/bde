// bdldfp_denselypackeddecimalimputil.cpp                             -*-C++-*-

#include <bdldfp_denselypackeddecimalimputil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$")

#include <bdldfp_decimalplatform.h>
#include <bdldfp_uint128.h>

#include <bsls_assert.h>
#include <bsl_algorithm.h>
#include <bsl_bitset.h>
#include <bsl_cstring.h>
#include <bsl_string.h>
#include <bsl_cstdlib.h>

#include <bslmf_assert.h>

// Even in hardware and intel modes, we need decNumber functions.

extern "C" {
#include <decSingle.h>
}

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
#  ifndef  __STDC_WANT_DEC_FP__
#    error __STDC_WANT_DEC_FP__ must be defined on the command line!
     BSLMF_ASSERT(false);
#  endif
#endif


namespace BloombergLP {
namespace bdldfp {

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

// Properties<32>, Properties<64>, and Properties<128> contain constants and
// member functions identifying key properties of these decimal types.
template <int Size>
struct Properties;

// Properties of the 32-bit decimal floating point type.
template <>
struct Properties<32>
{
    typedef unsigned StorageType;

    static const int         digits          = 7;
    static const int         combinationSize = 11;
    static const int         bias            = 101;
    static const int         maxExponent     = 90;
    static const StorageType signBit         = 0x80000000ul;
    static const long long   smallLimit      = 1000000ll;
    static const long long   mediumLimit     = 10000000ll;

    static inline StorageType getSignBit();
        // Return a 'StorageType' bit-package encoding a sign bit for 32-bit
        // DPD values.

    static inline unsigned long long lowDigits(unsigned long long value);
        // Return a 64-bit unsigned integer representing the low order decimal
        // digits in the specified 'value' capable of being encoded in declet
        // fields.  Digits which must be encoded into a combination field are
        // removed.

    static inline unsigned int topDigit(unsigned long long value);
        // Return an unsigned integer representing the high order decimal digit
        // of the specified 'value' capable of being encoded in a combination
        // field.  Digits which must be encoded into declet fields field are
        // removed.
};

inline Properties<32>::StorageType Properties<32>::getSignBit()
{
    return signBit;
}

inline unsigned long long Properties<32>::lowDigits(unsigned long long value)
{
    return value % smallLimit;
}

inline unsigned int Properties<32>::topDigit(unsigned long long value)
{
    return static_cast<unsigned int>(value / smallLimit);
}

// Properties of the 64-bit decimal floating point type.
template <>
struct Properties<64>
{
    typedef unsigned long long StorageType;

    static const int         digits          = 16;
    static const int         combinationSize = 13;
    static const int         bias            = 398;
    static const int         maxExponent     = 369;
    static const long long   smallLimit      = 1000000000000000ll;
    static const long long   mediumLimit     = 10000000000000000ll;
    static const StorageType signBit         = 0x8000000000000000ull;
    static const StorageType infBits         = 0x7800000000000000ull;
    static const StorageType plusInfBits     = 0x7800000000000000ull;
    static const StorageType minusInfBits    = 0xF800000000000000ull;

    static inline StorageType getSignBit();
        // Return a 'StorageType' bit-package encoding a sign bit for 32-bit
        // DPD values.

    static inline unsigned long long lowDigits(unsigned long long value);
        // Return a 64-bit unsigned integer representing the low order decimal
        // digits in the specified 'value' capable of being encoded in declet
        // fields.  Digits which must be encoded into a combination field are
        // removed.

    static inline unsigned int topDigit(unsigned long long value);
        // Return an unsigned integer representing the high order decimal digit
        // of the specified 'value' capable of being encoded in a combination
        // field.  Digits which must be encoded into declet fields field are
        // removed.
};

inline Properties<64>::StorageType Properties<64>::getSignBit()
{
    return signBit;
}

inline unsigned long long Properties<64>::lowDigits(unsigned long long value)
{
    return value % smallLimit;
}

inline unsigned int Properties<64>::topDigit(unsigned long long value)
{
    return static_cast<unsigned int>(value / smallLimit);
}

// Properties of the 128-bit decimal floating point type.
template <>
struct Properties<128>
{
    // Decimal floating point types has machine-dependent endianess.

    typedef BloombergLP::bdldfp::Uint128                       StorageType;

    static const int digits          = 34;
    static const int combinationSize = 17;
    static const int bias            = 6176;
    static const int maxExponent     = 6111;

    static inline StorageType getSignBit();
        // Return a 'StorageType' bit-package encoding a sign bit for 32-bit
        // DPD values.

    static inline unsigned long long lowDigits(unsigned long long value);
        // Return a 64-bit unsigned integer representing the low order decimal
        // digits in the specified 'value' capable of being encoded in declet
        // fields.  Digits which must be encoded into a combination field are
        // removed.

    static inline unsigned int topDigit(unsigned long long value);
        // Return an unsigned integer representing the high order decimal digit
        // of the specified 'value' capable of being encoded in a combination
        // field.  Digits which must be encoded into declet fields field are
        // removed.
};

inline Properties<128>::StorageType Properties<128>::getSignBit()
{
    StorageType signBit(0x8000000000000000ull, 0ull);
    return signBit;
}

inline unsigned long long Properties<128>::lowDigits(unsigned long long value)
{
    return value;
}

inline unsigned int Properties<128>::topDigit(unsigned long long)
{
    return 0;
}

template <int Size>
static typename Properties<Size>::StorageType getDeclets(
                                                      unsigned long long value)
    // Create the binary-encoded declets of the representing the specified
    // 'value', excluding the leading digit of 'value' which needs to be
    // encoded in a combination field.
{
    typedef typename Properties<Size>::StorageType StorageType;

    unsigned int shift(0u);
    StorageType bits = StorageType();
    for (; value; value /= 1000ull, shift += 10u) {
        bits |=
            StorageType(DenselyPackedDecimalImpUtil::encodeDeclet(
                                                   unsigned(value % 1000ull)))
            << shift;
    }
    return bits;
}

                        // combination field functions

template <int Size>
static typename Properties<Size>::StorageType makeCombinationField(
                                                         unsigned int digit,
                                                         int          exponent)
    // Create the binary-encoded combination field, which combines the
    // specified 'exponent' with the specified leading 'digit' of the mantissa.
{
    // lower (size - 5) bits: the bits of the exp with the bias addded top 5
    // bits: G0...G4
    //
    //: o top two bits of the exponent (E0E1)
    //: o four bits of the digit (D0...D3)
    //
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
    unsigned long expo(exponent + Properties<Size>::bias);

    typename Properties<Size>::StorageType exp(expo & clear);
    exp |= digit < 8u
        ? (((expo & (0x3ul << shift)) << 3u)
           | (digit << shift))
        : ((0x18ul << shift)
           | ((expo & (0x3ul << shift)) << 1u)
           | (digit & 0x1u) << shift);

    return exp <<= (Size - size - 1);
}

template <int Size>
static typename Properties<Size>::StorageType combineDecimalRaw(
                                                   unsigned long long mantissa,
                                                   int                exponent,
                                                   bool               negative)
    // Return the a pattern of bits representing a decimal floating point value
    // specified by the given 'mantissa', 'exponent', and 'negative'.
{
    typedef typename Properties<Size>::StorageType StorageType;
    StorageType combinationField(makeCombinationField<Size>(
                              Properties<Size>::topDigit(mantissa), exponent));

    StorageType sign = negative ? Properties<Size>::getSignBit() : 0;

    return sign |
           combinationField |
           getDeclets<Size>(Properties<Size>::lowDigits(mantissa));   // RETURN
}

template <int Size>
static typename Properties<Size>::StorageType toDecimalRaw(
                                                   unsigned long long mantissa,
                                                   int                exponent,
                                                   bool               negative)
    // Return the a pattern of bits representing a decimal floating point value
    // specified by the given 'mantissa', 'exponent', and 'negative'.
{
    return combineDecimalRaw<Size>(mantissa, exponent, negative);
}

template <int Size>
void toDecimalRaw(typename Properties<Size>::StorageType *target,
                  unsigned long long                      mantissa)
    // Assign to the specified 'target' a pattern of bits representing a
    // decimal floating point value specified by the given 'mantissa'.
{
    return toDecimalRaw<Size>(mantissa, 0, false);
}

                        // makeDecimalRaw implementation functions

template<int Size>
void makeDecimalRaw(typename Properties<Size>::StorageType *target,
                    unsigned long long                      mantissa,
                    int                                     exponent)
    // Assign to the specified 'target' a pattern of bits representing a
    // decimal floating point value specified by the given 'mantissa', and
    // 'exponent'.
{
    *target = toDecimalRaw<Size>(mantissa, exponent, false);
}

template<int Size>
void makeDecimalRaw(typename Properties<Size>::StorageType *target,
                    signed long long                        mantissa,
                    int                                     exponent)
    // Assign to the specified 'target' a pattern of bits representing a
    // decimal floating point value specified by the given 'mantissa', and
    // 'exponent'.
{
    typename Properties<Size>::StorageType bits;

    if (mantissa == std::numeric_limits<long long>::min()) {
        bits = toDecimalRaw<Size>(static_cast<unsigned long long>(
                   std::numeric_limits<long long>::max()) + 1, exponent, true);
    }
    else {
        bits = toDecimalRaw<Size>(bsl::max(-mantissa, mantissa),
                                  exponent,
                                  mantissa < 0);
    }

    *target = bits;
}

template<int Size>
void makeDecimalRaw(typename Properties<Size>::StorageType *target,
                    unsigned int                            mantissa,
                    int                                     exponent)
    // Assign to the specified 'target' a pattern of bits representing a
    // decimal floating point value specified by the given 'mantissa', and
    // 'exponent'.
{
    makeDecimalRaw<Size>(target,
                         static_cast<unsigned long long>(mantissa),
                         exponent);
}

template<int Size>
void makeDecimalRaw(typename Properties<Size>::StorageType *target,
                    signed int                              mantissa,
                    int                                     exponent)
    // Assign to the specified 'target' a pattern of bits representing a
    // decimal floating point value specified by the given 'mantissa', and
    // 'exponent'.
{
    makeDecimalRaw<Size>(target,
                         static_cast<signed long long>(mantissa),
                         exponent);
}

}  // close unnamed namespace

                        // Declet encoding functions

unsigned DenselyPackedDecimalImpUtil::encodeDeclet(unsigned digits)
{
    BSLS_ASSERT(digits < 1000);
    return declets[digits];
}

                        // Declet decoding functions

unsigned DenselyPackedDecimalImpUtil::decodeDeclet(unsigned declet)
{
    BSLS_ASSERT(declet < 1024);
    const unsigned short *loc= std::find(declets, declets + 1000, declet);

    // Undefined behavior, if declet isn't in DPD format.  We require that
    // there are zeros in the "don't care" bits.

    BSLS_ASSERT(loc != declets + 1000);

    return loc - declets;
}

DenselyPackedDecimalImpUtil::StorageType32
DenselyPackedDecimalImpUtil::makeDecimalRaw32(int mantissa,
                                              int exponent)
{
    BSLS_ASSERT(-101     <= exponent);
    BSLS_ASSERT(exponent <= 90);
    BSLS_ASSERT(bsl::max(mantissa, -mantissa) <= 9999999);

    StorageType32 storagetype32;
    makeDecimalRaw<32>(&storagetype32, mantissa, exponent);
    return storagetype32;
}

DenselyPackedDecimalImpUtil::StorageType64
DenselyPackedDecimalImpUtil::makeDecimalRaw64(unsigned long long mantissa,
                                              int                exponent)
{
    BSLS_ASSERT(-398     <= exponent);
    BSLS_ASSERT(exponent <= 369);
    BSLS_ASSERT(mantissa <= 9999999999999999LL);

    StorageType64 storagetype64;
    makeDecimalRaw<64>(&storagetype64, mantissa, exponent);
    return storagetype64;
}

DenselyPackedDecimalImpUtil::StorageType64
DenselyPackedDecimalImpUtil::makeDecimalRaw64(long long mantissa,
                                              int       exponent)
{
    BSLS_ASSERT(-398     <= exponent);
    BSLS_ASSERT(exponent <= 369);
    BSLS_ASSERT(std::max(mantissa, -mantissa) <= 9999999999999999LL);

    StorageType64 storagetype64;
    makeDecimalRaw<64>(&storagetype64, mantissa, exponent);
    return storagetype64;
}

DenselyPackedDecimalImpUtil::StorageType64
DenselyPackedDecimalImpUtil::makeDecimalRaw64(unsigned int mantissa,
                                              int          exponent)
{
    BSLS_ASSERT(-398     <= exponent);
    BSLS_ASSERT(exponent <= 369);

    StorageType64 storagetype64;
    makeDecimalRaw<64>(&storagetype64, mantissa, exponent);
    return storagetype64;
}

DenselyPackedDecimalImpUtil::StorageType64
DenselyPackedDecimalImpUtil::makeDecimalRaw64(int mantissa,
                                              int exponent)
{
    BSLS_ASSERT(-398     <= exponent);
    BSLS_ASSERT(exponent <= 369);

    StorageType64 storagetype64;
    makeDecimalRaw<64>(&storagetype64, mantissa, exponent);
    return storagetype64;
}

DenselyPackedDecimalImpUtil::StorageType128
DenselyPackedDecimalImpUtil::makeDecimalRaw128(unsigned long long mantissa,
                                               int                exponent)
{
    BSLS_ASSERT(-6176    <= exponent);
    BSLS_ASSERT(exponent <= 6111);

    StorageType128 storagetype128;
    makeDecimalRaw<128>(&storagetype128, mantissa, exponent);
    return storagetype128;
}

DenselyPackedDecimalImpUtil::StorageType128
DenselyPackedDecimalImpUtil::makeDecimalRaw128(long long  mantissa,
                                               int        exponent)
{
    BSLS_ASSERT(-6176    <= exponent);
    BSLS_ASSERT(exponent <= 6111);

    StorageType128 storagetype128;
    makeDecimalRaw<128>(&storagetype128, mantissa, exponent);
    return storagetype128;
}

DenselyPackedDecimalImpUtil::StorageType128
DenselyPackedDecimalImpUtil::makeDecimalRaw128(unsigned int mantissa,
                                               int          exponent)
{
    BSLS_ASSERT(-6176    <= exponent);
    BSLS_ASSERT(exponent <= 6111);

    StorageType128 storagetype128;
    makeDecimalRaw<128>(&storagetype128, mantissa, exponent);
    return storagetype128;
}

DenselyPackedDecimalImpUtil::StorageType128
DenselyPackedDecimalImpUtil::makeDecimalRaw128(int mantissa,
                                               int exponent)
{
    BSLS_ASSERT(-6176    <= exponent);
    BSLS_ASSERT(exponent <= 6111);

    StorageType128 storagetype128;
    makeDecimalRaw<128>(&storagetype128, mantissa, exponent);
    return storagetype128;
}

}  // close package namespace
}  // close enterprise namespace


// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
