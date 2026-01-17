#include "common.h"



int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);

/*
    int curr_idx = 0;
	while(curr_idx+1 < float_form.nr_sample_points) {
		int new_idx = 0;
		float val = 0.0f;
		maximum_abs_value_and_position_float_array(&float_form.samples[curr_idx], float_form.nr_sample_points - curr_idx, &new_idx, &val);
		new_idx += curr_idx;
		fprintf(stderr, "curr: %i, new: %i\n", curr_idx, new_idx);
		for(int i = curr_idx+1; i <= new_idx; i++) {
			float_form.samples[i] = val;
		}
		if(curr_idx == new_idx) break;
		curr_idx = new_idx;
	}
*/	
	float* begin = float_form.samples;
	float* curr = float_form.samples;
	float* end = &float_form.samples[float_form.nr_sample_points-1];
	int remaining = float_form.nr_sample_points;
	while(curr < end) {
		int max_idx;
		float val;
		maximum_abs_value_and_position_float_array(curr, remaining, &max_idx, &val);
		if(curr != begin) {
			curr[-1] = val;
		}
		for(int i = 0; i <= max_idx; i++) {
			curr[i] = val;
		}
		/*fprintf(stderr, "curr: %p, idx: %i\n", curr, max_idx);*/
		curr = &curr[max_idx+1];
		remaining -= max_idx;
	}
	
	
	for(int i = 0; i < float_form.nr_sample_points-1; i++) {
		if(float_form.samples[i] < float_form.samples[i+1]) {
			fprintf(stderr, "test violated at: %i!\n", i);
		}
	}
	
    
    write_simple_wav(stdout, float_form);

    return 0;
}
