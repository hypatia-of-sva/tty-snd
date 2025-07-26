#include "common.h"

int main(int argc, char** argv) {
    assert(argc >= 2);

    float stretch = atof(argv[1]);
    assert(1.0f <= stretch && stretch <= 2.0f);

    simple_wav_t float_form = read_simple_wav(stdin);

    simple_wav_t out_form;
    out_form.frequency_in_hz = 2*float_form.frequency_in_hz;
    out_form.nr_sample_points = 2*float_form.nr_sample_points;
    out_form.samples = calloc(sizeof(float), out_form.nr_sample_points);
    int maximum_written_index = (int) (((float)out_form.nr_sample_points)*(stretch/4.0f));
    fprintf(stderr, "size: %li, max index: %i\n", out_form.nr_sample_points, maximum_written_index);
    for(int i = 0; i < maximum_written_index; i++) {
        int original_index = floor((float)i*(1.0f/stretch));
        fprintf(stderr, "[%i]=%i,", i, original_index);
        out_form.samples[i] = float_form.samples[original_index];
        out_form.samples[out_form.nr_sample_points-i] = out_form.samples[i];
    }

    write_simple_wav(stdout, out_form);


    return 0;
}
