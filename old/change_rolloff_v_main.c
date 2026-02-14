#include "common.h"

void squish_single_peak(float* data, interval_t region, float squish_factor) {
    size_t len = region.upper_index-region.lower_index;
    float* new_data = calloc(len, sizeof(float));
    if (squish_factor == 1.0f) { /* do nothing */ }
    else if(squish_factor < 1.0f) {
        for(int i = 0; i < len; i++) {
            size_t equiv_index = region.lower_index+i*squish_factor;
            new_data[i] = data[equiv_index];
        }
    } else {
        size_t max_idx = floor((float)len / (float) squish_factor);
        for(int i = 0; i < max_idx; i++) {
            size_t equiv_index = region.lower_index+i*squish_factor;
            new_data[i] = data[equiv_index];
        }
        /* the rest is already zeroed by calloc */
    }
    for(int i = 0; i < len; i++) {
        data[region.lower_index+i] = new_data[i];
    }
    free(new_data);
}

void squish_peaks(simple_wav_t form, float squish_factor) {
    for(int i = 1; i < form.nr_peaks; i++) {
        interval_t region;
        region.lower_index = form.peaks[i].underlying_interval.upper_index;
        region.upper_index = form.peaks[i].min_index;
        fprintf(stderr, "of peak [%zu,%zu] to %i: ", form.peaks[i].underlying_interval.lower_index, form.peaks[i].underlying_interval.upper_index, form.peaks[i].min_index);
        fprintf(stderr, "squish [%zu,%zu]\n", region.lower_index, region.upper_index);
        squish_single_peak(form.samples, region, squish_factor);
    }
}


int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);

    assert(argc > 1);
    float squish_factor = atof(argv[1]);

    if(float_form.nr_peaks == 0) {
        fprintf(stderr, "No peaks found!ßn");
    } else {
        fprintf(stderr, "Peaks found!\n");
        fprintf(stderr, "Squishing peaks with factor: %f\n", squish_factor);
        squish_peaks(float_form, squish_factor);
    }


    write_simple_wav(stdout, float_form);


    return 0;
}


/*
    float frequency_in_hz;
    size_t nr_sample_points;
    float* samples;
    size_t nr_peaks;
    peak_t* peaks;



typedef struct peak_t {
    interval_t underlying_interval;
    float freq;
    float height;
    int formant_nr;
    int merged_peaks;
    float rolloff_v;
    int min_index;
} peak_t;
typedef struct interval_t {
    size_t lower_index;
    size_t upper_index;
} interval_t;
*/
