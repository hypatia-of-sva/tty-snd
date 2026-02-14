#include "common.h"



int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);

    fprintf(stderr, "writing raw to stdout (freq = %f)\n", float_form.frequency_in_hz);
    for(int i = 0; i < float_form.nr_sample_points; i++) {
        float real_val = sqrt(float_form.samples[2*i]*float_form.samples[2*i]
                        + float_form.samples[2*i+1]*float_form.samples[2*i+1]);
        int16_t int_val = (int16_t) (real_val + (float)((1<<16)-1));
        fputc(int_val & 0xFF);
        fputc((int_val & 0xFF00) >> 8);
    }


    return 0;
}
