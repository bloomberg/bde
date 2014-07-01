#include <bdldfp_decimalimputil.h>

namespace BloombergLP {
namespace bdldfp {

DecimalImpUtil::ValueType64 DecimalImpUtil::makeDecimal64(int coeff, int exponent)
{
    return DecimalImpUtil::makeDecimalRaw64(coeff, exponent);
}

DecimalImpUtil::ValueType64 DecimalImpUtil::makeDecimal64(unsigned int coeff, int exponent)
{
    return DecimalImpUtil::makeDecimalRaw64(coeff, exponent);
}

DecimalImpUtil::ValueType64 DecimalImpUtil::makeDecimal64(long long coeff, int exponent)
{
    return DecimalImpUtil::makeDecimalRaw64(coeff, exponent);
}

DecimalImpUtil::ValueType64 DecimalImpUtil::makeDecimal64(unsigned long long coeff, int exponent)
{
    return DecimalImpUtil::makeDecimalRaw64(coeff, exponent);
}

}
}
