#include "common.h"



int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);

    for(int i = 0; i < float_form.nr_peaks; i++) {

    }

    if(float_form.nr_peaks == 0) {
        fprintf(stderr, "No peaks found!");
    } else {
        fprintf(stderr, "Peaks found!");
        debug_peaks(float_form.peaks, float_form.nr_peaks);
    }


    write_simple_wav(stdout, float_form);


    return 0;
}
