#include "common.h"

/* tty-snd-fft:
        read in a series of floats and return their fft
*/




int main(int argc, char** argv) {

    assert(argc >= 3 && strcmp(argv[0], "-w") != 0);

    int size_red = atoi(argv[1]); // power of two
    int index = atoi(argv[2]);

    assert(index < (1<<size_red));

    simple_wav_t float_form = read_simple_wav(stdin);


    fprintf(stderr, "before: nr = %li, ptr = %p\n", float_form.nr_sample_points, float_form.samples);

    float_form.nr_sample_points >>= size_red;
    float_form.samples = &(float_form.samples[float_form.nr_sample_points*index]);

    fprintf(stderr, "after: nr = %li, ptr = %p\n", float_form.nr_sample_points, float_form.samples);

    float window_param = 1.0f;
    if(argc > 3 && strcmp(argv[3], "-w") == 0) {
        window_param = atof(argv[4]);
        fprintf(stderr, "recognized param: %f\n", window_param);
    }

    if(window_param != 1.0f)
    for(int i = 0; i < float_form.nr_sample_points; i++) {
        float_form.samples[i] *= window_param - (1-window_param)*cos(2*M_PI / float_form.nr_sample_points);
    }


    float* fft_array = ifft_power_of_two(float_form.samples, float_form.nr_sample_points);

    simple_wav_t out_form;
    out_form.frequency_in_hz = float_form.frequency_in_hz;
    out_form.nr_sample_points = float_form.nr_sample_points;
    out_form.samples = fft_array;


    //fprintf(stderr, "fft: f = %f\n", float_form.frequency_in_hz);

    write_simple_wav(stdout, out_form);


    return 0;
}
