#include "common.h"



int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);

    simple_wav_t out_form;
    out_form.frequency_in_hz = float_form.frequency_in_hz;
    out_form.nr_sample_points = float_form.nr_sample_points/2;
    out_form.samples = calloc(sizeof(float),out_form.nr_sample_points);
    for(int i = 0; i < out_form.nr_sample_points; i++) {
        out_form.samples[i] = float_form.samples[2*i]; /*sqrt(float_form.samples[2*i]*float_form.samples[2*i]
                        + float_form.samples[2*i+1]*float_form.samples[2*i+1]) */;
    }

    write_simple_wav(stdout, out_form);


    return 0;
}
