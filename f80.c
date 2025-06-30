#include "common.h"


/* portable util functions for extended float (80 bit IEEE 754, Intel x87 format)
 * based on the table in https://en.wikipedia.org/wiki/Extended_precision (10.5.25)
 * since the intel docs are too confusing
 */
void convert_to_extended_float_be(double val, char* outptr) {
    uint64_t vaL_bitcast    = ((uint64_t*)(&val))[0];
    uint8_t  sign_bit       = (vaL_bitcast & 0x8000000000000000LL)>>63;
    uint16_t old_exponent   = (vaL_bitcast & 0x7FF0000000000000LL)>>52;
    uint64_t old_mantissa   = (vaL_bitcast & 0x000FFFFFFFFFFFFFLL);
    uint8_t  new_integer_bit;
    uint16_t new_exponent;
    uint64_t new_mantissa;
    if(old_exponent == 0) {
        if(old_mantissa == 0) {
            /* a) +-zero */
            new_integer_bit = 0;
            new_exponent = 0;
            new_mantissa = 0;
        } else {
            /* b) subnormals */
            /* these are not subnormals in 80 bit! */
            int highest_set_bit_in_mantissa = -1;
            for(int i = 0; i < 52; i++) {
                if(old_mantissa & (1LL<<i)) highest_set_bit_in_mantissa = i;
            }
            assert(highest_set_bit_in_mantissa > 0);
            new_integer_bit = 1;
            new_exponent = old_exponent-1023+16383 + (highest_set_bit_in_mantissa-51);
            new_mantissa = (old_mantissa-(1<<highest_set_bit_in_mantissa))
                                << (11 + highest_set_bit_in_mantissa+1);
        }
    } else if (old_exponent == 0x7FF) {
        if(old_mantissa == 0) {
            /* c) +-infinity */
            new_integer_bit = 1;
            new_exponent = 0x7FFF;
            new_mantissa = 0;
        } else {
            /* d) NaNs */
            new_integer_bit = 1;
            new_exponent = 0x7FFF;
            new_mantissa = old_mantissa << 11;
        }
    } else {
        /* e) normal numbers */
        new_integer_bit = 1;
        new_exponent = old_exponent-1023+16383;
        new_mantissa = old_mantissa << 11;
    }

    assert(outptr != NULL);
    outptr[0] = (sign_bit<<7) | ((new_exponent&0x7F00)>>8);
    outptr[1] = (new_exponent&0x00FF);
    outptr[2] = (new_integer_bit<<7) | ((new_mantissa&0x7F00000000000000LL)>>56);
    outptr[3] = ((new_mantissa&0x00FF000000000000LL)>>48);
    outptr[4] = ((new_mantissa&0x0000FF0000000000LL)>>40);
    outptr[5] = ((new_mantissa&0x000000FF00000000LL)>>32);
    outptr[6] = ((new_mantissa&0x00000000FF000000LL)>>24);
    outptr[7] = ((new_mantissa&0x0000000000FF0000LL)>>16);
    outptr[8] = ((new_mantissa&0x000000000000FF00LL)>>8);
    outptr[9] = ((new_mantissa&0x00000000000000FFLL));
}
double convert_from_extended_float_be(char* inptr) {
    uint8_t  sign_bit;
    uint16_t old_exponent;
    uint64_t old_mantissa;
    uint8_t  old_integer_bit;
    uint16_t new_exponent;
    uint64_t new_mantissa;
    uint64_t new_double;


    assert(inptr != NULL);

    sign_bit = 0x80&inptr[0];
    old_exponent = ((0x7F&inptr[0])<<8) | inptr[1];
    (void) (old_integer_bit = 0x80&inptr[2]);
    old_mantissa =  (((uint64_t) (0x7F&inptr[2]))<<56);
    old_mantissa |= (((uint64_t) (0x7F&inptr[3]))<<48);
    old_mantissa |= (((uint64_t) (0x7F&inptr[4]))<<40);
    old_mantissa |= (((uint64_t) (0x7F&inptr[5]))<<32);
    old_mantissa |= (((uint64_t) (0x7F&inptr[6]))<<24);
    old_mantissa |= (((uint64_t) (0x7F&inptr[7]))<<16);
    old_mantissa |= (((uint64_t) (0x7F&inptr[8]))<<8);
    old_mantissa |= ((0x7F&inptr[9]));

    if(old_exponent == 0) {
        /* a) +- zero or subnormals - they are so small they will be rounded to 0*/
        new_exponent = 0;
        new_mantissa = 0;
    } else if (old_exponent == 0x7FFF) {
        /* b) infinity or NaN - just propagate */
        new_exponent = 0x7FF;
        new_mantissa = old_mantissa >> 11;
    } else {
        /* c) normal numbers: */
        int16_t signed_exponent = (int16_t)(old_exponent-16383);
        if(signed_exponent > 1024) {
            /* i) too big to be represented - set to +-infinity*/
            new_exponent = 0x7FF;
            new_mantissa = 0;
        } else if (signed_exponent < -1022) {
            /* ii) too small to be represented - set to a subnormal, potentially 0 */
            new_exponent = 0;
            new_mantissa = old_mantissa >> (11-signed_exponent);
        } else {
            new_exponent = (uint16_t)(signed_exponent+1023);
            new_mantissa = old_mantissa >> 11;
        }
    }

    new_double = ((uint64_t)(sign_bit | ((new_exponent&0x07F0)>>4)))<<56;
    new_double |= ((new_exponent&0x00F) | (new_mantissa&0x000F000000000000LL))<<48;
    new_double |= (new_mantissa&0x0000FF0000000000LL)<<40;
    new_double |= (new_mantissa&0x000000FF00000000LL)<<32;
    new_double |= (new_mantissa&0x00000000FF000000LL)<<24;
    new_double |= (new_mantissa&0x0000000000FF0000LL)<<16;
    new_double |= (new_mantissa&0x000000000000FF00LL)<<8;
    new_double |= (new_mantissa&0x00000000000000FFLL);

    return ((double*)(&new_double))[0];
}
