
#include "common.h"



int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);


    assert(argc > 2);
    float min_f = atof(argv[1]);
    float max_f = atof(argv[2]);
    /*
    float min_amp = atof(argv[3]);
    float max_amp = atof(argv[4]);
    */
    assert(min_f < max_f);
    /*assert(min_amp < max_amp);*/


    /*
    simple_wav_t out_form = {0};
    out_form.frequency_in_hz = float_form.frequency_in_hz;
    out_form.nr_sample_points = float_form.nr_sample_points;
    out_form.samples = calloc(sizeof(float),out_form.nr_sample_points);
    */


    size_t n = float_form.nr_sample_points;
    float* p = float_form.samples;

    int min_index, max_index;
    if(min_f < 0.0f) {
        min_index = 0;
    } else if (min_f > float_form.frequency_in_hz/2.0) {
        min_index = n/2.0;
    } else {
        min_index = (min_f/float_form.frequency_in_hz) * n;
    }
    if(max_f < 0.0f) {
        max_index = 0;
    } else if (max_f > float_form.frequency_in_hz/2.0) {
        max_index = n/2.0;
    } else {
        max_index = (max_f/float_form.frequency_in_hz) * n;
    }


    memset((void*)p, 0, min_index*sizeof(float));
    memset((void*)&p[max_index], 0, ((n/2)-max_index)*sizeof(float));


    memset((void*)&p[n-min_index], 0, min_index*sizeof(float));
    memset((void*)&p[n/2], 0, ((n/2)-max_index)*sizeof(float));

    /* bc they actually refer to complex numbers */
    /*
    min_index /= 2;
    max_index /= 2;

    for(int i = min_index; i < max_index; i++) {
        float real = float_form.samples[2*i];
        float imag = float_form.samples[2*i+1];
        float abs_value = real*real + imag*imag;*/

        /*
        if(abs_value < min_amp) {
            real = 0.0f;
            imag = 0.0f;
        } else if(abs_value > max_amp) {
            real *= max_amp / abs_value;
            imag *= max_amp / abs_value;
        }
        */
        /*
        out_form.samples[2*i] = real;
        out_form.samples[2*i+1] = imag;
        out_form.samples[(out_form.nr_sample_points)-2*i-1] = real;
        out_form.samples[(out_form.nr_sample_points)-2*i] = imag;
    }
    */


    write_simple_wav(stdout, float_form);


    return 0;
}
