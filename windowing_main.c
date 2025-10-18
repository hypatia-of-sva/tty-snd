
#include "common.h"



int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);


    assert(argc > 1);
    float param = atof(argv[1]);

    simple_wav_t out_form = {0};
    out_form.frequency_in_hz = float_form.frequency_in_hz;
    out_form.nr_sample_points = float_form.nr_sample_points;
    out_form.samples = calloc(sizeof(float),out_form.nr_sample_points);

    for(int i = 0; i < out_form.nr_sample_points/2; i++) {
        float real = float_form.samples[2*i];
        float imag = float_form.samples[2*i+1];

        float window = param - (1 - param)*cos(2*M_PI*i / (out_form.nr_sample_points/2));

        out_form.samples[2*i] = window*real;
        out_form.samples[2*i+1] = window*imag;
    }

    write_simple_wav(stdout, out_form);


    return 0;
}
