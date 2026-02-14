#include "common.h"



int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);

    if(float_form.nr_peaks == 0) {
        fprintf(stderr, "No peaks found!\n");
    } else {
        fprintf(stderr, "Peaks found!\nPRINTOUT:\n");
        debug_peaks(float_form.peaks, float_form.nr_peaks);
    }


    write_simple_wav(stdout, float_form);


    return 0;
}
