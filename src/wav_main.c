#include "common.h"

int main(int argc, char** argv) {
    assert(argc >= 3);

    waveform_t form = read_amplitude_data(argv[1], atoi(argv[2]));
    size_t len = truncate_power_of_2(form.data_length);
    float freq = ((float)form.samples_per_second);
    if(!is_power_of_2(len)) die("Data size collected not power of two!");

    float* amplitudes = transform_to_complex_array(form.amplitude_data, len);

    simple_wav_t float_form = {0};
    float_form.frequency_in_hz = freq;
    float_form.nr_sample_points = 2*len;
    float_form.samples = amplitudes;



    //fprintf(stderr, "wav: f = %f\n", freq);

    write_simple_wav(stdout, float_form);



    return 0;
}
