#include <stdint.h>
#include <string.h>

#include "ieee754_clf.h"

union DoubleToUint64 {
    double d;
    uint64_t u;
};

float_class_t classify(double x) {
    union DoubleToUint64 converter;
    converter.d = x;
    uint64_t bits = converter.u;
    int bit_of_sign = (bits >> 63) & 1;
    uint64_t exponent = (bits >> 52) & 0x7FF;
    uint64_t fraction = bits & 0xFFFFFFFFFFFFF;

    if (exponent == 0 && fraction == 0) {
        return (bit_of_sign == 0) ? Zero : MinusZero;
    } else if (exponent == 0x7FF && fraction == 0) {
        return (bit_of_sign == 0) ? Inf : MinusInf;
    } else if (exponent == 0x7FF && fraction != 0) {
        return NaN;
    } else if (exponent == 0 && fraction != 0) {
        return (bit_of_sign == 0) ? Denormal : MinusDenormal;
    } else {
        return (bit_of_sign == 0) ? Regular : MinusRegular;
    }
    return Zero;
}
