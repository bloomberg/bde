#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL
#define INCLUDED_BDLDFP_DECIMALIMPUTIL

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL_INTELDFP
#include <bdldfp_decimalimputil_inteldfp.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL_DECNUMBER
#include <bdldfp_decimalimputil_decnumber.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL_IBMXLC
#include <bdldfp_decimalimputil_ibmxlc.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#include <bdldfp_decimalplatform.h>
#endif

#if BDLDFP_DECIMALPLATFORM_SOFTWARE

                // DECIMAL FLOATING-POINT LITERAL EMULATION


#define BDLDFP_DECIMALIMPUTIL_DF(lit)                                        \
    BloombergLP::bdldfp::DecimalImpUtil::parse32(                            \
        (BloombergLP::bdldfp::DecimalImpUtil::checkLiteral(lit), #lit))

#define BDLDFP_DECIMALIMPUTIL_DD(lit)                                        \
    BloombergLP::bdldfp::DecimalImpUtil::parse64(                            \
        (BloombergLP::bdldfp::DecimalImpUtil::checkLiteral(lit), #lit))

#define BDLDFP_DECIMALIMPUTIL_DL(lit)                                        \
    BloombergLP::bdldfp::DecimalImpUtil::parse128(                           \
        (BloombergLP::bdldfp::DecimalImpUtil::checkLiteral(lit), #lit))

#elif BDLDFP_DECIMALPLATFORM_C99_TR || defined( __IBM_DFP__ )

#define BDLDFP_DECIMALIMPUTIL_JOIN_(a,b) a##b

               // Portable decimal floating-point literal support

#define BDLDFP_DECIMALIMPUTIL_DF(lit) BDLDFP_DECIMALIMPUTIL_JOIN_(lit,df)

#define BDLDFP_DECIMALIMPUTIL_DD(lit) BDLDFP_DECIMALIMPUTIL_JOIN_(lit,dd)

#define BDLDFP_DECIMALIMPUTIL_DL(lit) BDLDFP_DECIMALIMPUTIL_JOIN_(lit,dl)

#endif


namespace BloombergLP {
namespace bdldfp {

class DecimalImpUtil : public BloombergLP::bdldfp::DecimalImpUtil_Platform
{
  public:
    static ValueType64 makeDecimal64(               int coeff, int exponent);
    static ValueType64 makeDecimal64(unsigned       int coeff, int exponent);
    static ValueType64 makeDecimal64(         long long coeff, int exponent);
    static ValueType64 makeDecimal64(unsigned long long coeff, int exponent);

    static ValueType64 makeInfinity64(bool isNegative = false);
        // Return a 'ValueType64' representing an infinity.  Optionally specify
        // whether the infinity 'isNegative'.  If 'isNegative' is not
        // specified, the infinite value will be positive.  Note that if
        // 'isNegative' is true, the infinite value will be negative, and
        // positive otherwise.

    // CLASS METHODS
#if BDLDFP_DECIMALPLATFORM_SOFTWARE

    struct This_is_not_a_floating_point_literal {};
        // This 'struct' is a helper type used togenerate error messages for
        // bad literals.

    template <class TYPE>
    static void checkLiteral(const TYPE& t);
        // Generate an error if the specified 't' is bad decimal
        // floating-point.  Note that this function is intended for use with
        // literals

    static void checkLiteral(double);
        // Overload to avoid an error when the decimal floating-point literal
        // (without the suffix) can be interpreted as a 'double' literal.

#elif BDLDFP_DECIMALPLATFORM_HARDWARE

#else

#error Improperly configured decimal floating point platform settings

#endif
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // --------------------
                          // class DecimalImpUtil
                          // --------------------

#if BDLDFP_DECIMALPLATFORM_SOFTWARE

template <class TYPE>
inline
void DecimalImpUtil::checkLiteral(const TYPE& t)
{
    (void)static_cast<This_is_not_a_floating_point_literal>(t);
}

inline
void DecimalImpUtil::checkLiteral(double)
{
}
#endif


}
}

#endif
