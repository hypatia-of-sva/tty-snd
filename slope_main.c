#include "common.h"



int main(int argc, char** argv) {
    assert(argc >= 2);

    simple_wav_t float_form = read_simple_wav(stdin);

    float percentage_drop_per_hz = atof(argv[1]);
    assert(percentage_drop_per_hz > 0.0 && percentage_drop_per_hz < 100.0);
    float multipler_per_sample = (percentage_drop_per_hz/100.0)*(float_form.frequency_in_hz / float_form.nr_sample_points);
    fprintf(stderr, "multiplier per sample = %f, max multiplier = %f", multipler_per_sample, fmax(1-float_form.nr_sample_points*multipler_per_sample, 0.0f));

    for(int i = 0; i < float_form.nr_sample_points; i++) {
        float_form.samples[i] *= fmax(1-i*multipler_per_sample, 0.0f);
    }

    write_simple_wav(stdout, float_form);


    return 0;
}
