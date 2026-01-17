#include "common.h"



int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);
    
    assert(argc > 1);
    float max_percentage = atof(argv[1]);
     
    int max_idx; float max_val;
    maximum_abs_value_and_position_float_array(float_form.samples, float_form.nr_sample_points, &max_idx, &max_val);
    
    float cutoff = max_val * max_percentage;
    int cutoff_index = 0;
    for(int i = 0; i < float_form.nr_sample_points; i++) {
		if(float_form.samples[i] <= cutoff) {
			break;
		}
		cutoff_index++;
	}
	
	fprintf(stderr, "per: %f, value: %f, idx: %i, max: %i\n", max_percentage, cutoff, cutoff_index, float_form.nr_sample_points);
	
	
	
	
	
    simple_wav_t out_form = {0};
    out_form.frequency_in_hz = float_form.frequency_in_hz;
    out_form.nr_sample_points = cutoff_index;
    out_form.samples = calloc(sizeof(float),out_form.nr_sample_points);
    for(int i = 0; i < out_form.nr_sample_points; i++) {
        out_form.samples[i] = float_form.samples[i];
    }

    write_simple_wav(stdout, out_form);


    return 0;
}

