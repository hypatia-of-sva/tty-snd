#include "common.h"

/* tty-snd-fft:
        read in a series of floats and return their fft
*/




int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);

    float* fft_array = fft_power_of_two(float_form.samples, float_form.nr_sample_points);

    simple_wav_t out_form = {float_form.frequency_in_hz, float_form.nr_sample_points, fft_array};

    write_simple_wav(stdout, out_form);

    return 0;
}
